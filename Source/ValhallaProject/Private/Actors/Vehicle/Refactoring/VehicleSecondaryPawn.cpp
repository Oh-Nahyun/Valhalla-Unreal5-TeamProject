// Fill out your copyright notice in the Description page of Project Settings.


#include "Actors/Vehicle/Refactoring/VehicleSecondaryPawn.h"

#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"
#include "Net/UnrealNetwork.h"

#include "Player/ValhallaPlayerController.h"
#include "Interface/SeatInterface.h"
#include "Actors/Vehicle/Refactoring/ValhallaVehicleBase.h"
#include "Actors/Vehicle/Refactoring/Skill/VehicleSkillProjectile.h"

AVehicleSecondaryPawn::AVehicleSecondaryPawn()
{
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = true;

	bReplicates = true;

	Mesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Mesh"));
	Mesh->BodyInstance.bSimulatePhysics = false;
	Mesh->BodyInstance.bNotifyRigidBodyCollision = true;
	Mesh->BodyInstance.bUseCCD = true;
	Mesh->SetGenerateOverlapEvents(true);
	Mesh->SetCanEverAffectNavigation(false);
	RootComponent = Mesh;
	Mesh->SetIsReplicated(true);

	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->SetRelativeLocation(FVector(0.f, 0.f, 55.f));
	CameraBoom->TargetArmLength = 1000.0f;
	CameraBoom->bUsePawnControlRotation = true;
	CameraBoom->bDoCollisionTest = true;

	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	FollowCamera->bUsePawnControlRotation = false;

	BallistaMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BallistaMesh"));
	BallistaMesh->SetupAttachment(RootComponent);
	BallistaMesh->SetIsReplicated(true);

	ShotLocation = CreateDefaultSubobject<USceneComponent>(TEXT("ShotLocation"));
	ShotLocation->SetupAttachment(BallistaMesh);
	ShotLocation->SetIsReplicated(true);

	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	VehiclePlayerController = nullptr;

	IsSkillNow = false;

	TeamType = EPlayerTeamType::None;

	ActorType = EActorType::Vehicle;
}

void AVehicleSecondaryPawn::BeginPlay()
{
	Super::BeginPlay();
	
	if (AValhallaVehicleBase* MyControlledVehicle = Cast<AValhallaVehicleBase>(GetParentActor()))
	{
		ControlledVehicle = MyControlledVehicle;
	}
}

void AVehicleSecondaryPawn::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);


}

void AVehicleSecondaryPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &AVehicleSecondaryPawn::SecondaryPawnLook);
		EnhancedInputComponent->BindAction(GetOffAction, ETriggerEvent::Started, this, &AVehicleSecondaryPawn::SecondaryGetOff);
	}
}

void AVehicleSecondaryPawn::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	Client_GetSubsystem(NewController);

	if (AValhallaPlayerController* NewPlayerController = Cast<AValhallaPlayerController>(NewController))
	{
		TeamType = Cast<ITeamInterface>(NewPlayerController->ControlledPawn)->GetActorTeamType();

		VehiclePlayerController = NewPlayerController;
	}
}

void AVehicleSecondaryPawn::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AVehicleSecondaryPawn, IsSkillNow);
	DOREPLIFETIME(AVehicleSecondaryPawn, VehiclePlayerController);
	DOREPLIFETIME(AVehicleSecondaryPawn, ControlledVehicle);
	DOREPLIFETIME(AVehicleSecondaryPawn, TeamType);
}

void AVehicleSecondaryPawn::Client_GetSubsystem_Implementation(AController* InController)
{
	if (APlayerController* NewPlayerController = Cast<APlayerController>(InController))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(NewPlayerController->GetLocalPlayer()))
		{
			Subsystem->ClearAllMappings();

			check(VehicleSecondaryMappingContext);
			Subsystem->AddMappingContext(VehicleSecondaryMappingContext, 0);
		}
	}
}

void AVehicleSecondaryPawn::SecondaryPawnLook(const FInputActionValue& Value)
{
	if (Controller)
	{
		FVector2D LookAxisVector = Value.Get<FVector2D>();

		AddControllerYawInput(LookAxisVector.X);
		AddControllerPitchInput(LookAxisVector.Y * -1.f);
	}
}

void AVehicleSecondaryPawn::SecondaryGetOff(const FInputActionValue& Value)
{
	if (VehiclePlayerController)
	{
		if (HasAuthority())
		{
			if (ISeatInterface* SeatInterface = Cast<ISeatInterface>(VehiclePlayerController->ControlledPawn))
			{
				SeatInterface->SetGetOffState();
			}

			MC_SecondaryGetOff();

			VehiclePlayerController->Possess(VehiclePlayerController->ControlledPawn);

			TeamType = EPlayerTeamType::None;

			if (AValhallaVehicleBase* PrimaryVehiclePawn = Cast<AValhallaVehicleBase>(GetParentActor()))
			{
				PrimaryVehiclePawn->SecondarySeat = nullptr;
			}
		}
		else
		{
			Server_SecondaryGetOff();
		}
	}
}

void AVehicleSecondaryPawn::ShotBallista()
{
	FVector SpawnLocation = ShotLocation->GetComponentLocation();
	FRotator SpawnRotator = ShotLocation->GetComponentRotation();
	FTransform SpawnTransform(SpawnRotator, SpawnLocation, FVector(1.f, 1.f, 1.f));

	AVehicleSkillProjectile* SkillProjectile = GetWorld()->SpawnActorDeferred<AVehicleSkillProjectile>(ControlledVehicle->SkillProjectileClass, SpawnTransform, this, this, ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn);
	if (SkillProjectile)
	{
		SkillProjectile->SetTeamType(TeamType);

		SkillProjectile->FinishSpawning(SpawnTransform);
	}
}

void AVehicleSecondaryPawn::Server_SecondaryGetOff_Implementation()
{
	if (VehiclePlayerController)
	{
		if (ISeatInterface* SeatInterface = Cast<ISeatInterface>(VehiclePlayerController->ControlledPawn))
		{
			SeatInterface->SetGetOffState();
		}

		MC_SecondaryGetOff();

		VehiclePlayerController->Possess(VehiclePlayerController->ControlledPawn);

		TeamType = EPlayerTeamType::None;

		if (AValhallaVehicleBase* PrimaryVehiclePawn = Cast<AValhallaVehicleBase>(GetParentActor()))
		{
			PrimaryVehiclePawn->SecondarySeat = nullptr;
		}
	}
}

void AVehicleSecondaryPawn::MC_SecondaryGetOff_Implementation()
{
	if (AValhallaVehicleBase* PrimaryVehiclePawn = Cast<AValhallaVehicleBase>(GetParentActor()))
	{
		if (VehiclePlayerController)
		{
			FVector GetOffLocation = PrimaryVehiclePawn->GetSecondarySeatGetOffLocation()->GetComponentLocation();
			VehiclePlayerController->ControlledPawn->SetActorLocation(GetOffLocation, false, nullptr, ETeleportType::ResetPhysics);
		}
	}
}

