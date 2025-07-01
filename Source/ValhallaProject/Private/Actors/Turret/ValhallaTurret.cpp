// Fill out your copyright notice in the Description page of Project Settings.


#include "Actors/Turret/ValhallaTurret.h"

#include "Components/BoxComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Net/UnrealNetwork.h"
#include "Components/WidgetComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Components/SphereComponent.h"
#include "Kismet/GameplayStatics.h"
#include "BehaviorTree/BlackboardComponent.h"

#include "AI/TurretAIController.h"
#include "Actors/Turret/ValhallaTurretBullet.h"
#include "Widget/ValhallaHealthWidget.h"
#include "Interface/GoldInterface.h"
#include "Player/ValhallaPlayerController.h"
#include "Interface/StateInterface.h"

AValhallaTurret::AValhallaTurret()
{
	PrimaryActorTick.bCanEverTick = true;

	TurretMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("TurretMesh"));
	SetRootComponent(TurretMesh);

	BulletSpawnTransform = CreateDefaultSubobject<USceneComponent>(TEXT("BulletSpawnTransform"));
	BulletSpawnTransform->SetupAttachment(RootComponent);

	TurretWidgetComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("ConstructionWidgetComponent"));
	TurretWidgetComponent->SetupAttachment(RootComponent);

	SphereCollision = CreateDefaultSubobject<USphereComponent>(TEXT("SphereCollision"));
	SphereCollision->SetupAttachment(RootComponent);
	SphereCollision->OnComponentBeginOverlap.AddDynamic(this, &AValhallaTurret::OnCollisionBeginOverlap);
	SphereCollision->OnComponentEndOverlap.AddDynamic(this, &AValhallaTurret::OnCollisionEndOverlap);

	TeamType = EPlayerTeamType::None;

	ActorType = EActorType::Turret;

	MaxHealth = 300.f;
	TurretDefense = 15.f;

	GoldGivenAmount = 150;
}

void AValhallaTurret::BeginPlay()
{
	Super::BeginPlay();
	
	check(TurretBulletClass);

	if (HasAuthority())
	{
		InitializeStatus();
	}

	UValhallaHealthWidget* HealthWidget = Cast<UValhallaHealthWidget>(TurretWidgetComponent->GetWidget());
	if (HealthWidget)
	{
		OnTurretHealthChangedDelegate.BindUObject(HealthWidget, &UValhallaHealthWidget::SetHealthBar);
	}
}

void AValhallaTurret::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AValhallaTurret::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void AValhallaTurret::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	if (ATurretAIController* TurretAIController = Cast<ATurretAIController>(NewController))
	{
		TurretController = TurretAIController;
	}
}

void AValhallaTurret::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AValhallaTurret, CurrentHealth);

	DOREPLIFETIME(AValhallaTurret, TargetActors);
	DOREPLIFETIME(AValhallaTurret, TargetActor);
}

float AValhallaTurret::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	if (EventInstigator == nullptr)
	{
		return 0.f;
	}

	if (ITeamInterface* TeamInterface = Cast<ITeamInterface>(EventInstigator->GetPawn()))
	{
		if (TeamInterface->GetActorTeamType() == TeamType)
		{
			return 0.f;
		}
	}

	Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

	float Damage = FMath::Clamp(DamageAmount - TurretDefense, 0, DamageAmount);

	if (HasAuthority())
	{
		if (Damage > 0)
		{
			float NewCurrentHealth = FMath::Clamp(CurrentHealth - Damage, 0, CurrentHealth);
			CurrentHealth = NewCurrentHealth;

			if (IActorTypeInterface* ActorTypeInterface = Cast<IActorTypeInterface>(EventInstigator->GetPawn<APawn>()))
			{
				if (ActorTypeInterface->GetActorTypeTag() == EActorType::Player || ActorTypeInterface->GetActorTypeTag() == EActorType::Vehicle)
				{
					OnTurretHealthChanged(CurrentHealth, MaxHealth, EventInstigator);
				}
			}
		}

		if (CurrentHealth <= 0)
		{
			if (AValhallaPlayerController* InstigatorController = Cast<AValhallaPlayerController>(EventInstigator))
			{
				if (IGoldInterface* PlayerCharacterGoldInterface = Cast<IGoldInterface>(InstigatorController->ControlledPawn))
				{
					PlayerCharacterGoldInterface->GainExpAndGoldInterface(0, GoldGivenAmount);
				}
			}

			DestroyTurret();
		}
	}

	return Damage;
}

void AValhallaTurret::Destroyed()
{
	Super::Destroyed();

	if (DestroySound != nullptr)
	{
		UGameplayStatics::PlaySoundAtLocation(this, DestroySound, GetActorLocation());
	}
}

void AValhallaTurret::OnCollisionBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (HasAuthority())
	{
		ITeamInterface* TargetTeamInterface = Cast<ITeamInterface>(OtherActor);

		if (TeamType == TargetTeamInterface->GetActorTeamType())
		{
			return;
		}

		TargetActors.AddUnique(OtherActor);

		float DetectedDistance = 0.f;
		TargetActor = UGameplayStatics::FindNearestActor(GetActorLocation(), TargetActors, DetectedDistance);
		TurretController->GetBlackboardComponent()->SetValueAsObject(TEXT("CurrentTarget"), TargetActor);
	}
}

void AValhallaTurret::OnCollisionEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (HasAuthority())
	{
		ITeamInterface* TargetTeamInterface = Cast<ITeamInterface>(OtherActor);

		if (TeamType == TargetTeamInterface->GetActorTeamType())
		{
			return;
		}

		TargetActors.RemoveSingle(OtherActor);

		if (TargetActors.IsEmpty())
		{
			TargetActor = nullptr;
			TurretController->GetBlackboardComponent()->SetValueAsObject(TEXT("CurrentTarget"), nullptr);
		}
		else
		{
			float DetectedDistance = 0.f;
			TargetActor = UGameplayStatics::FindNearestActor(GetActorLocation(), TargetActors, DetectedDistance);
			TurretController->GetBlackboardComponent()->SetValueAsObject(TEXT("CurrentTarget"), TargetActor);
		}
	}
}

void AValhallaTurret::InitializeStatus()
{
	CurrentHealth = MaxHealth;
}

void AValhallaTurret::DestroyTurret()
{
	Destroy();
}

EPlayerTeamType& AValhallaTurret::GetActorTeamType()
{
	return TeamType;
}

EActorType& AValhallaTurret::GetActorTypeTag()
{
	return ActorType;
}

void AValhallaTurret::ResetTarget()
{
	if (TargetActors.IsEmpty())
	{
		TargetActor = nullptr;
		return;
	}

	float Distance = 2200.f;

	for (AActor* Target : TargetActors)
	{
		float TargetDistance = UKismetMathLibrary::Vector_Distance(GetActorLocation(), Target->GetActorLocation());

		if (Distance >= TargetDistance)
		{
			TargetActor = Target;
			Distance = TargetDistance;
		}
	}
}

void AValhallaTurret::OnTurretHealthChanged_Implementation(float InCurrentHealth, float InMaxHealth, AController* InInstigatorController)
{
	OnTurretHealthChangedDelegate.ExecuteIfBound(InCurrentHealth, InMaxHealth, InInstigatorController);
}

void AValhallaTurret::BulletFire()
{
	if (TargetActor == nullptr)
	{
		return;
	}

	if (IActorTypeInterface* ActorTypeInterface = Cast<IActorTypeInterface>(TargetActor))
	{
		if (ActorTypeInterface->GetActorTypeTag() == EActorType::Player)
		{
			if (IStateInterface* StateInterface = Cast<IStateInterface>(ActorTypeInterface))
			{
				if (StateInterface->GetPlayerState() == EPlayerCurrentState::Dead)
				{
					TargetActors.RemoveSingle(TargetActor);

					ResetTarget();
				}
			}
		}
	}

	if (TargetActor == nullptr)
	{
		return;
	}

	FVector SpawnLocation = BulletSpawnTransform->GetComponentLocation();
	FVector SpawnRelativeLocation = BulletSpawnTransform->GetRelativeLocation();

	FRotator SpawnRotator = BulletSpawnTransform->GetComponentRotation();
	FTransform SpawnTransform(SpawnRotator, SpawnLocation, FVector(1.f, 1.f, 1.f));

	AValhallaTurretBullet* Bullet = GetWorld()->SpawnActorDeferred<AValhallaTurretBullet>(TurretBulletClass, SpawnTransform, this, this, ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn);
	if (Bullet)
	{
		Bullet->SetTeamType(TeamType);

		FVector TargetLocation = TargetActor->GetActorLocation() + FVector(0.f, 0.f, -25.f);
		FRotator BulletRot = UKismetMathLibrary::FindLookAtRotation(SpawnTransform.GetLocation(), TargetLocation);
		Bullet->SetActorRotation(BulletRot);

		Bullet->FinishSpawning(SpawnTransform);
	}
}

