// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/ValhallaMinion.h"

#include "Net/UnrealNetwork.h"
#include "Components/WidgetComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/SphereComponent.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Components/BoxComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"

#include "Widget/ValhallaHealthWidget.h"
#include "Interface/GoldInterface.h"
#include "Actors/Weapon/RangeMinionProjectile.h"
#include "Player/ValhallaPlayerController.h"

AValhallaMinion::AValhallaMinion()
{
	CharacterHealthWidgetComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("CharacterHealthWidgetComponent"));
	CharacterHealthWidgetComponent->SetupAttachment(RootComponent);

	SphereCollision = CreateDefaultSubobject<USphereComponent>(TEXT("SphereCollision"));
	SphereCollision->SetupAttachment(RootComponent);
	SphereCollision->OnComponentBeginOverlap.AddDynamic(this, &AValhallaMinion::OnCollisionBeginOverlap);
	SphereCollision->OnComponentEndOverlap.AddDynamic(this, &AValhallaMinion::OnCollisionEndOverlap);

	ActorType = EActorType::Minion;

	GoldGivenAmount = 20;
	ExpGivenAmount = 15.f;

	MaxHealth = 250.f;

	MeleeMinionDamage = 100.f;

	MinionType = EMinionType::Melee;

	WeaponMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("WeaponMesh"));
	WeaponMesh->SetupAttachment(GetMesh(), TEXT("WeaponSocket"));

	WeaponCollisionBox = CreateDefaultSubobject<UBoxComponent>(TEXT("WeaponCollisionBox"));
	WeaponCollisionBox->SetupAttachment(WeaponMesh);
	WeaponCollisionBox->SetBoxExtent(FVector(20.f));
	WeaponCollisionBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	WeaponCollisionBox->OnComponentBeginOverlap.AddDynamic(this, &ThisClass::OnWeaponCollisionBeginOverlap);
}

void AValhallaMinion::BeginPlay()
{
	Super::BeginPlay();

	if (TeamType == EPlayerTeamType::Blue)
	{
		SphereCollision->SetCollisionResponseToChannel(ECC_GameTraceChannel4, ECollisionResponse::ECR_Ignore);
		WeaponCollisionBox->SetCollisionResponseToChannel(ECC_GameTraceChannel4, ECollisionResponse::ECR_Ignore);

		GetCapsuleComponent()->SetCollisionObjectType(ECC_GameTraceChannel4);
	}
	else if (TeamType == EPlayerTeamType::Red)
	{
		SphereCollision->SetCollisionResponseToChannel(ECC_GameTraceChannel3, ECollisionResponse::ECR_Ignore);
		WeaponCollisionBox->SetCollisionResponseToChannel(ECC_GameTraceChannel3, ECollisionResponse::ECR_Ignore);

		GetCapsuleComponent()->SetCollisionObjectType(ECC_GameTraceChannel3);
	}

	UValhallaHealthWidget* HealthWidget = Cast<UValhallaHealthWidget>(CharacterHealthWidgetComponent->GetWidget());
	if (HealthWidget)
	{
		OnCharacterHealthChangedDelegate.BindUObject(HealthWidget, &UValhallaHealthWidget::SetHealthBar);
	}
}

void AValhallaMinion::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	InitializeHealthAndMana();
}

void AValhallaMinion::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AValhallaMinion, MaxHealth);
	DOREPLIFETIME(AValhallaMinion, ExpGivenAmount);
	DOREPLIFETIME(AValhallaMinion, TargetActors);
	DOREPLIFETIME(AValhallaMinion, TargetActor);
	DOREPLIFETIME(AValhallaMinion, IsMinionDead);
}

float AValhallaMinion::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	if (EventInstigator == nullptr || PlayerCurrentState == EPlayerCurrentState::Dead)
	{
		return 0.f;
	}

	APawn* InstigatorPawn = EventInstigator->GetPawn();

	if (ITeamInterface* TeamInterface = Cast<ITeamInterface>(InstigatorPawn))
	{
		if (TeamInterface->GetActorTeamType() == TeamType)
		{
			return 0.f;
		}
	}

	Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

	if (HasAuthority())
	{
		DamagedMinionHealth(DamageAmount);

		if (IActorTypeInterface* ActorTypeInterface = Cast<IActorTypeInterface>(InstigatorPawn))
		{
			if (ActorTypeInterface->GetActorTypeTag() == EActorType::Player || ActorTypeInterface->GetActorTypeTag() == EActorType::Vehicle)
			{
				OnCharacterHealthChanged(CurrentHealth, MaxHealth, EventInstigator);
			}
		}

		if (IsDead(CurrentHealth))
		{
			Die();
			OnRep_PlayerCurrentState();

			if (AValhallaPlayerController* InstigatorController = Cast<AValhallaPlayerController>(EventInstigator))
			{
				if (IGoldInterface* PlayerCharacterGoldInterface = Cast<IGoldInterface>(InstigatorController->ControlledPawn))
				{
					PlayerCharacterGoldInterface->GainExpAndGoldInterface(ExpGivenAmount, GoldGivenAmount);
				}
			}
		}
	}

	return DamageAmount;
}

void AValhallaMinion::OnCollisionBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (HasAuthority())
	{
		ITeamInterface* TargetTeamInterface = Cast<ITeamInterface>(OtherActor);
		if (TargetTeamInterface->GetActorTeamType() == TeamType || TargetTeamInterface->GetActorTeamType() == EPlayerTeamType::None)
		{
			return;
		}

		if (!IsValid(OtherActor))
		{
			return;
		}

		TargetActors.AddUnique(OtherActor);

		if (!TargetActors.IsEmpty())
		{
			if (AAIController* MinionController = Cast<AAIController>(GetController()))
			{
				MinionController->GetBlackboardComponent()->SetValueAsInt(TEXT("IsTargetInRange"), 1);
			}
		}
	}
}

void AValhallaMinion::OnCollisionEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (HasAuthority())
	{
		ITeamInterface* TargetTeamInterface = Cast<ITeamInterface>(OtherActor);
		if (TargetTeamInterface->GetActorTeamType() == TeamType)
		{
			return;
		}

		TargetActors.RemoveSingle(OtherActor);

		if (TargetActors.IsEmpty())
		{
			if (AAIController* MinionController = Cast<AAIController>(GetController()))
			{
				MinionController->GetBlackboardComponent()->SetValueAsInt(TEXT("IsTargetInRange"), 0);
				TargetActor = nullptr;
			}
		}
		else
		{
			if (AAIController* MinionController = Cast<AAIController>(GetController()))
			{
				MinionController->GetBlackboardComponent()->SetValueAsInt(TEXT("IsTargetInRange"), 1);
			}
		}
	}
}

void AValhallaMinion::InitializeHealthAndMana()
{
	CurrentHealth = MaxHealth;
}

void AValhallaMinion::DamagedMinionHealth(const float InDamage)
{
	float NewCurrentHealth = FMath::Clamp(CurrentHealth - InDamage, 0, CurrentHealth);
	CurrentHealth = NewCurrentHealth;
}

void AValhallaMinion::Die()
{
	Super::Die();

	GetController()->UnPossess();

	PlayerCurrentState = EPlayerCurrentState::Dead;

	IsMinionDead = true;
	OnRep_IsMinionDead();
}

void AValhallaMinion::OnRep_IsMinionDead()
{
	if (IsMinionDead)
	{
		SetCollisionOff();
	}
}

void AValhallaMinion::SetCollisionOff()
{
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECollisionChannel::ECC_GameTraceChannel3, ECollisionResponse::ECR_Ignore);
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECollisionChannel::ECC_GameTraceChannel4, ECollisionResponse::ECR_Ignore);
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECollisionChannel::ECC_GameTraceChannel2, ECollisionResponse::ECR_Ignore);
}

void AValhallaMinion::DestroyMinion()
{
	Destroy();
}

void AValhallaMinion::ToggleWeaponCollision(bool IsCollisionEnable)
{
	Super::ToggleWeaponCollision(IsCollisionEnable);

	if (IsCollisionEnable)
	{
		WeaponCollisionBox->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	}
	else
	{
		WeaponCollisionBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}
}

void AValhallaMinion::OnWeaponCollisionBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (HasAuthority())
	{
		AController* DamageInstigator = GetController();

		UGameplayStatics::ApplyDamage(OtherActor, MeleeMinionDamage, DamageInstigator, this, nullptr);
	}
	
	if (ITeamInterface* OtherTeamInterface = Cast<ITeamInterface>(OtherActor))
	{
		if (OtherTeamInterface->GetActorTeamType() == TeamType)
		{
			return;
		}
	}

	if (HasAuthority())
	{
		if (IActorTypeInterface* ActorTypeInterface = Cast<IActorTypeInterface>(OtherActor))
		{
			if (ActorTypeInterface->GetActorTypeTag() == EActorType::Player || ActorTypeInterface->GetActorTypeTag() == EActorType::Minion)
			{

			}
			else if (ActorTypeInterface->GetActorTypeTag() == EActorType::Construction || ActorTypeInterface->GetActorTypeTag() == EActorType::Turret || ActorTypeInterface->GetActorTypeTag() == EActorType::GatewayTurret)
			{
				ServerHitEffect(OtherActor);
			}
		}
	}
}

void AValhallaMinion::RangeAttack()
{
	if (TargetActor == nullptr)
	{
		return;
	}

	FVector SpawnLocation = GetMesh()->GetSocketLocation(TEXT("RangeMinionAttackSocket"));
	FRotator SpawnRotator = GetMesh()->GetSocketRotation(TEXT("RangeMinionAttackSocket"));
	FTransform SpawnTransform(SpawnRotator, SpawnLocation, FVector(1.f, 1.f, 1.f));

	ARangeMinionProjectile* Bullet = GetWorld()->SpawnActorDeferred<ARangeMinionProjectile>(RangeMinionProjectileClass, SpawnTransform, this, this, ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn);
	if (Bullet)
	{
		Bullet->SetTeamType(TeamType);
		Bullet->SetBulletCollision(TeamType);

		FVector TargetLocation = TargetActor->GetActorLocation();
		FRotator BulletRot = UKismetMathLibrary::FindLookAtRotation(SpawnTransform.GetLocation(), TargetLocation);
		Bullet->SetActorRotation(BulletRot);

		Bullet->FinishSpawning(SpawnTransform);
	}
}

void AValhallaMinion::OnCharacterHealthChanged_Implementation(float InCurrentHealth, float InMaxHealth, AController* InInstigatorController)
{
	OnCharacterHealthChangedDelegate.ExecuteIfBound(InCurrentHealth, InMaxHealth, InInstigatorController);
}

void AValhallaMinion::PlayHitSound_Implementation()
{
	if (HitSound)
	{
		UGameplayStatics::PlaySoundAtLocation(this, HitSound, GetActorLocation());
	}
}
