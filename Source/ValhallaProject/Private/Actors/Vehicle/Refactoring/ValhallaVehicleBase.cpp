// Fill out your copyright notice in the Description page of Project Settings.


#include "Actors/Vehicle/Refactoring/ValhallaVehicleBase.h"

#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "Net/UnrealNetwork.h"
#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"
#include "Components/WidgetComponent.h"
#include "Components/ChildActorComponent.h"
#include "ChaosVehicleMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"

#include "Widget/ValhallaHealthWidget.h"
#include "Player/ValhallaPlayerController.h"
#include "Interface/SeatInterface.h"
#include "Interface/GoldInterface.h"
#include "Actors/Vehicle/Refactoring/VehicleSecondaryPawn.h"
#include "Actors/Vehicle/Refactoring/Skill/VehicleSkillProjectile.h"

AValhallaVehicleBase::AValhallaVehicleBase()
{
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = true;

	bReplicates = true;

	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->SetRelativeLocation(FVector(0.f, 0.f, 55.f));
	CameraBoom->TargetArmLength = 1000.0f;
	CameraBoom->bUsePawnControlRotation = true;
	CameraBoom->bDoCollisionTest = true;

	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	FollowCamera->bUsePawnControlRotation = false;

	VehicleWidgetComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("VehicleWidgetComponent"));
	VehicleWidgetComponent->SetupAttachment(RootComponent);

	PrimarySeatGetOffLocation = CreateDefaultSubobject<USceneComponent>(TEXT("PrimarySeatGetOffLocation"));
	PrimarySeatGetOffLocation->SetupAttachment(RootComponent);

	SecondarySeatGetOffLocation = CreateDefaultSubobject<USceneComponent>(TEXT("SecondarySeatGetOffLocation"));
	SecondarySeatGetOffLocation->SetupAttachment(RootComponent);

	AttackPartMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("AttackPartMesh"));
	AttackPartMesh->SetupAttachment(RootComponent);
	AttackPartMesh->OnComponentBeginOverlap.AddDynamic(this, &AValhallaVehicleBase::OnMeshBeginOverlap);
	AttackPartMesh->SetIsReplicated(true);

	SecondaryPawn = CreateDefaultSubobject<UChildActorComponent>(TEXT("ChildActorComponent"));
	SecondaryPawn->SetupAttachment(RootComponent);
	SecondaryPawn->SetIsReplicated(true);

	TurretDetectLocation = CreateDefaultSubobject<USceneComponent>(TEXT("TurretDetectLocation"));
	TurretDetectLocation->SetupAttachment(RootComponent);

	TeamType = EPlayerTeamType::None;

	ActorType = EActorType::Vehicle;

	MaxHealth = 2000.f;
	SiegeWeaponDefense = 10.f;

	GoldGivenAmount = 150;

	PrimarySeat = nullptr;
	SecondarySeat = nullptr;

	VehiclePlayerController = nullptr;

	AttackDamage = 150.f;
}

void AValhallaVehicleBase::BeginPlay()
{
	Super::BeginPlay();

	if (HasAuthority())
	{
		InitializeStatus();
	}

	UValhallaHealthWidget* HealthWidget = Cast<UValhallaHealthWidget>(VehicleWidgetComponent->GetWidget());
	if (HealthWidget)
	{
		OnVehicleHealthChangedDelegate.BindUObject(HealthWidget, &UValhallaHealthWidget::SetHealthBar);
	}

	if (AVehicleSecondaryPawn* Secondary = Cast<AVehicleSecondaryPawn>(SecondaryPawn->GetChildActor()))
	{
		VehicleSecondaryPawn = Secondary;
	}
}

void AValhallaVehicleBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (Controller)
	{
		FRotator PrimaryRot = FRotator::ZeroRotator;
		PrimaryRot.Pitch = GetControlRotation().Pitch;

		CameraBoom->SetRelativeRotation(PrimaryRot);
	}
}

void AValhallaVehicleBase::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	Client_GetSubsystem(NewController);

	if (AValhallaPlayerController* NewPlayerController = Cast<AValhallaPlayerController>(NewController))
	{
		TeamType = Cast<ITeamInterface>(NewPlayerController->ControlledPawn)->GetActorTeamType();

		VehiclePlayerController = NewPlayerController;

		FRotator VehicleRot = FRotator::ZeroRotator;
		VehicleRot.Pitch = GetActorRotation().Pitch;
		VehicleRot.Yaw = GetActorRotation().Yaw;
		VehicleRot.Roll = GetActorRotation().Roll;

		VehiclePlayerController->SetControlRotation(VehicleRot);
	}
}

void AValhallaVehicleBase::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		EnhancedInputComponent->BindAction(VehicleLookAction, ETriggerEvent::Triggered, this, &AValhallaVehicleBase::VehicleLook);
		EnhancedInputComponent->BindAction(AttackAction, ETriggerEvent::Started, this, &AValhallaVehicleBase::VehicleAttack);
		EnhancedInputComponent->BindAction(GetOffAction, ETriggerEvent::Started, this, &AValhallaVehicleBase::VehicleGetOff);
	}
}

void AValhallaVehicleBase::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AValhallaVehicleBase, CurrentHealth);
	DOREPLIFETIME(AValhallaVehicleBase, TeamType);

	DOREPLIFETIME(AValhallaVehicleBase, PrimarySeat);
	DOREPLIFETIME(AValhallaVehicleBase, SecondarySeat);

	DOREPLIFETIME(AValhallaVehicleBase, VehiclePlayerController);
	DOREPLIFETIME(AValhallaVehicleBase, VehicleSecondaryPawn);
}

float AValhallaVehicleBase::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	if (EventInstigator == nullptr /*|| TeamType == EPlayerTeamType::None*/)
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

	float Damage = FMath::Clamp(DamageAmount - SiegeWeaponDefense, 0, DamageAmount);

	if (HasAuthority())
	{
		APawn* InstigatorPawn = EventInstigator->GetPawn();

		if (Damage > 0)
		{
			float NewCurrentHealth = FMath::Clamp(CurrentHealth - Damage, 0, CurrentHealth);
			CurrentHealth = NewCurrentHealth;

			if (IActorTypeInterface* ActorTypeInterface = Cast<IActorTypeInterface>(InstigatorPawn))
			{
				if (ActorTypeInterface->GetActorTypeTag() == EActorType::Player || ActorTypeInterface->GetActorTypeTag() == EActorType::Vehicle)
				{
					OnVehicleHealthChanged(CurrentHealth, MaxHealth, EventInstigator);
				}

				if (ActorTypeInterface->GetActorTypeTag() == EActorType::Vehicle)
				{
					PlayVehicleDamagedSound();
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

			DestroyVehicle();
		}
	}

	return Damage;
}

void AValhallaVehicleBase::Destroyed()
{
	Super::Destroyed();

}

void AValhallaVehicleBase::UnPossessed()
{
	Super::UnPossessed();

	VehiclePlayerController = nullptr;
}

void AValhallaVehicleBase::Client_GetSubsystem_Implementation(AController* InController)
{
	if (APlayerController* NewPlayerController = Cast<APlayerController>(InController))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(NewPlayerController->GetLocalPlayer()))
		{
			Subsystem->ClearAllMappings();

			check(VehiclePrimaryMappingContext);
			Subsystem->AddMappingContext(VehiclePrimaryMappingContext, 0);
		}
	}

	FRotator VehicleRot = FRotator::ZeroRotator;
	VehicleRot.Pitch = GetActorRotation().Pitch;
	VehicleRot.Yaw = GetActorRotation().Yaw;
	VehicleRot.Roll = GetActorRotation().Roll;

	InController->SetControlRotation(VehicleRot);
}

void AValhallaVehicleBase::OnMeshBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
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
				ServerHitEffect(ActorTypeInterface->GetActorTypeTag());
			}
			else if (ActorTypeInterface->GetActorTypeTag() == EActorType::Construction || ActorTypeInterface->GetActorTypeTag() == EActorType::Turret || ActorTypeInterface->GetActorTypeTag() == EActorType::GatewayTurret)
			{
				ServerHitEffect(ActorTypeInterface->GetActorTypeTag());
			}
			/*
			else if (ActorTypeInterface->GetActorTypeTag() == EActorType::Vehicle)
			{
				ServerHitEffect(ActorTypeInterface->GetActorTypeTag());
			}
			*/
		}
	}
}

void AValhallaVehicleBase::VehicleLook(const FInputActionValue& Value)
{
	if (Controller)
	{
		FVector2D LookAxisVector = Value.Get<FVector2D>();

		AddControllerPitchInput(LookAxisVector.Y * -1.f);
	}
}

void AValhallaVehicleBase::VehicleAttack(const FInputActionValue& Value)
{
	if (Controller)
	{
		Attack();
	}
}

void AValhallaVehicleBase::VehicleGetOff(const FInputActionValue& Value)
{
	if (VehiclePlayerController)
	{
		GetVehicleMovementComponent()->StopMovementImmediately();

		if (HasAuthority())
		{
			if (ISeatInterface* SeatInterface = Cast<ISeatInterface>(VehiclePlayerController->ControlledPawn))
			{
				SeatInterface->SetGetOffState();
			}

			GetVehicleMovementComponent()->SetYawInput(0.f);
			GetVehicleMovementComponent()->SetThrottleInput(0.f);
			GetVehicleMovementComponent()->SetBrakeInput(1.f);

			MC_VehicleGetOff();

			VehiclePlayerController->Possess(VehiclePlayerController->ControlledPawn);
			PrimarySeat = nullptr;

			TeamType = EPlayerTeamType::None;
		}
		else
		{
			Server_VehicleGetOff();
		}
	}
}

void AValhallaVehicleBase::InitializeStatus()
{
	CurrentHealth = MaxHealth;
}

void AValhallaVehicleBase::OnVehicleHealthChanged_Implementation(float InCurrentHealth, float InMaxHealth, AController* InInstigatorController)
{
	OnVehicleHealthChangedDelegate.ExecuteIfBound(InCurrentHealth, InMaxHealth, InInstigatorController);
}

ESeatType AValhallaVehicleBase::GetSeat()
{
	if (PrimarySeat == nullptr)
	{
		return ESeatType::Primary;
	}
	else if (SecondarySeat == nullptr)
	{
		return ESeatType::Secondary;
	}
	else
	{
		return ESeatType::NoSeat;
	}
}

void AValhallaVehicleBase::SetVehicleSeat(AActor* InActor, const ESeatType& InSeatType)
{
	if (InSeatType == ESeatType::Primary)
	{
		PrimarySeat = InActor;
	}
	else if (InSeatType == ESeatType::Secondary)
	{
		SecondarySeat = InActor;
	}
}

AActor* AValhallaVehicleBase::GetSecondaryPawn()
{
	return SecondaryPawn->GetChildActor();
}

void AValhallaVehicleBase::DestroyVehicle()
{
	if (PrimarySeat != nullptr)
	{
		if (VehiclePlayerController)
		{
			if (ISeatInterface* SeatInterface = Cast<ISeatInterface>(VehiclePlayerController->ControlledPawn))
			{
				SeatInterface->SetGetOffState();
			}

			PrimarySeat = nullptr;

			VehiclePlayerController->Possess(VehiclePlayerController->ControlledPawn);
		}

		TeamType = EPlayerTeamType::None;
	}

	if (SecondarySeat != nullptr)
	{
		if (VehicleSecondaryPawn)
		{
			if (AValhallaPlayerController* SecondaryController = Cast<AValhallaPlayerController>(VehicleSecondaryPawn->GetController()))
			{
				if (ISeatInterface* SeatInterface = Cast<ISeatInterface>(SecondaryController->ControlledPawn))
				{
					SeatInterface->SetGetOffState();
				}

				SecondarySeat = nullptr;

				SecondaryController->Possess(SecondaryController->ControlledPawn);
			}
		}

		VehicleSecondaryPawn->SetTeamType(EPlayerTeamType::None);
	}

	Destroy();
}

void AValhallaVehicleBase::ApplyAttackDamage(AActor* InActor)
{
	if (IActorTypeInterface* ActorTypeInterface = Cast<IActorTypeInterface>(InActor))
	{
		if (ActorTypeInterface->GetActorTypeTag() == EActorType::Construction || ActorTypeInterface->GetActorTypeTag() == EActorType::Turret || ActorTypeInterface->GetActorTypeTag() == EActorType::GatewayTurret)
		{
			AController* DamageInstigator = GetController();
			float FinalDamage = AttackDamage * 1.5f;
			UGameplayStatics::ApplyDamage(InActor, FinalDamage, DamageInstigator, this, nullptr);
		}
		else if (ActorTypeInterface->GetActorTypeTag() == EActorType::Player || ActorTypeInterface->GetActorTypeTag() == EActorType::Minion)
		{
			AController* DamageInstigator = GetController();
			float FinalDamage = AttackDamage;
			UGameplayStatics::ApplyDamage(InActor, FinalDamage, DamageInstigator, this, nullptr);
		}
		else if (ActorTypeInterface->GetActorTypeTag() == EActorType::Vehicle)
		{
			AController* DamageInstigator = GetController();
			float FinalDamage = AttackDamage * 1.2f;
			UGameplayStatics::ApplyDamage(InActor, FinalDamage, DamageInstigator, this, nullptr);
		}
	}
}

bool AValhallaVehicleBase::IsStopNow()
{
	float MoveSpeed = GetVehicleMovementComponent()->GetForwardSpeed();
	
	return UKismetMathLibrary::NearlyEqual_FloatFloat(MoveSpeed, 0.f, 10.f);
}

void AValhallaVehicleBase::ShotSkill()
{
	if (!VehicleSecondaryPawn->GetController())
	{
		return;
	}

	for (const FName& ShotPoint : SkillRightLocations)
	{
		FVector SpawnLocation = GetMesh()->GetSocketLocation(ShotPoint);
		FRotator SpawnRotator = GetMesh()->GetSocketRotation(ShotPoint);
		FTransform SpawnTransform(SpawnRotator, SpawnLocation, FVector(1.f, 1.f, 1.f));

		AVehicleSkillProjectile* SkillProjectile = GetWorld()->SpawnActorDeferred<AVehicleSkillProjectile>(SkillProjectileClass, SpawnTransform, VehicleSecondaryPawn, VehicleSecondaryPawn, ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn);
		if (SkillProjectile)
		{
			SkillProjectile->SetTeamType(VehicleSecondaryPawn->GetActorTeamType());

			SkillProjectile->FinishSpawning(SpawnTransform);
		}
	}

	for (const FName& ShotPoint : SkillLeftLocations)
	{
		FVector SpawnLocation = GetMesh()->GetSocketLocation(ShotPoint);
		FRotator SpawnRotator = GetMesh()->GetSocketRotation(ShotPoint);
		FTransform SpawnTransform(SpawnRotator, SpawnLocation, FVector(1.f, 1.f, 1.f));

		AVehicleSkillProjectile* SkillProjectile = GetWorld()->SpawnActorDeferred<AVehicleSkillProjectile>(SkillProjectileClass, SpawnTransform, VehicleSecondaryPawn, VehicleSecondaryPawn, ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn);
		if (SkillProjectile)
		{
			SkillProjectile->SetTeamType(VehicleSecondaryPawn->GetActorTeamType());

			SkillProjectile->FinishSpawning(SpawnTransform);
		}
	}
}

void AValhallaVehicleBase::PlayVehicleDamagedSound_Implementation()
{
	if (DamagedSound != nullptr)
	{
		UGameplayStatics::SpawnSoundAtLocation(this, DamagedSound, GetActorLocation());
	}
}

void AValhallaVehicleBase::Server_VehicleGetOff_Implementation()
{
	if (VehiclePlayerController)
	{
		if (ISeatInterface* SeatInterface = Cast<ISeatInterface>(VehiclePlayerController->ControlledPawn))
		{
			SeatInterface->SetGetOffState();
		}

		GetVehicleMovementComponent()->SetYawInput(0.f);
		GetVehicleMovementComponent()->SetThrottleInput(0.f);
		GetVehicleMovementComponent()->SetBrakeInput(1.f);

		MC_VehicleGetOff();

		VehiclePlayerController->Possess(VehiclePlayerController->ControlledPawn);

		PrimarySeat = nullptr;

		TeamType = EPlayerTeamType::None;
	}
}

void AValhallaVehicleBase::MC_VehicleGetOff_Implementation()
{
	if (VehiclePlayerController)
	{
		FVector GetOffLocation = PrimarySeatGetOffLocation->GetComponentLocation();
		VehiclePlayerController->ControlledPawn->SetActorLocation(GetOffLocation, false, nullptr, ETeleportType::ResetPhysics);
	}
}
