// Fill out your copyright notice in the Description page of Project Settings.


#include "Actors/Vehicle/ValhallaSiegeWeapon.h"

#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "Net/UnrealNetwork.h"
#include "Components/WidgetComponent.h"
#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"
#include "ChaosVehicleMovementComponent.h"
#include "Kismet/GameplayStatics.h"

#include "Widget/ValhallaHealthWidget.h"
#include "Interface/GoldInterface.h"
#include "Player/ValhallaPlayerController.h"
#include "Interface/SeatInterface.h"
#include "Actors/Vehicle/CannonBullet.h"

#include "Others/Debug.h"

AValhallaSiegeWeapon::AValhallaSiegeWeapon()
{
	PrimaryActorTick.bCanEverTick = false;
	PrimaryActorTick.bStartWithTickEnabled = false;

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
	AttackPartMesh->OnComponentBeginOverlap.AddDynamic(this, &AValhallaSiegeWeapon::OnMeshBeginOverlap);
	AttackPartMesh->SetIsReplicated(true);

	BulletShotLocation = CreateDefaultSubobject<USceneComponent>(TEXT("BulletShotLocation"));
	BulletShotLocation->SetupAttachment(AttackPartMesh);

	TurretDetectLocation = CreateDefaultSubobject<USceneComponent>(TEXT("TurretDetectLocation"));
	TurretDetectLocation->SetupAttachment(RootComponent);

	TeamType = EPlayerTeamType::None;

	ActorType = EActorType::Vehicle;

	MaxHealth = 2000.f;
	SiegeWeaponDefense = 10.f;

	GoldGivenAmount = 150;

	IsAttackingNow = false;

	AttackDamage = 150.f;
}

void AValhallaSiegeWeapon::BeginPlay()
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

	TeamType = EPlayerTeamType::None;
}

void AValhallaSiegeWeapon::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	Client_GetSubsystem(NewController);

	if (AValhallaPlayerController* NewPlayerController = Cast<AValhallaPlayerController>(NewController))
	{
		TeamType = Cast<ITeamInterface>(NewPlayerController->ControlledPawn)->GetActorTeamType();
	}

	//ResetPerception();
}

void AValhallaSiegeWeapon::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		/*
		EnhancedInputComponent->BindAction(VehicleMoveForwardAction, ETriggerEvent::Triggered, this, &AValhallaSiegeWeapon::VehicleMoveForwardTriggered);
		EnhancedInputComponent->BindAction(VehicleMoveForwardAction, ETriggerEvent::Started, this, &AValhallaSiegeWeapon::VehicleMoveForwardStarted);
		EnhancedInputComponent->BindAction(VehicleMoveForwardAction, ETriggerEvent::Canceled, this, &AValhallaSiegeWeapon::VehicleMoveForwardStarted);

		EnhancedInputComponent->BindAction(VehicleTurnRightAction, ETriggerEvent::Triggered, this, &AValhallaSiegeWeapon::VehicleTurnRightTriggered);
		EnhancedInputComponent->BindAction(VehicleTurnRightAction, ETriggerEvent::Started, this, &AValhallaSiegeWeapon::VehicleTurnRightStarted);
		EnhancedInputComponent->BindAction(VehicleTurnRightAction, ETriggerEvent::Completed, this, &AValhallaSiegeWeapon::VehicleTurnRightCompleted);
		EnhancedInputComponent->BindAction(VehicleTurnRightAction, ETriggerEvent::Canceled, this, &AValhallaSiegeWeapon::VehicleTurnRightCompleted);

		EnhancedInputComponent->BindAction(VehicleLookAction, ETriggerEvent::Triggered, this, &AValhallaSiegeWeapon::VehicleLook);
		*/

		EnhancedInputComponent->BindAction(GetOffAction, ETriggerEvent::Started, this, &AValhallaSiegeWeapon::GetOff);

		EnhancedInputComponent->BindAction(AttackAction, ETriggerEvent::Started, this, &AValhallaSiegeWeapon::VehicleAttack);
	}
}

void AValhallaSiegeWeapon::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AValhallaSiegeWeapon, CurrentHealth);
	DOREPLIFETIME(AValhallaSiegeWeapon, TeamType);

	DOREPLIFETIME(AValhallaSiegeWeapon, IsAttackingNow);

	DOREPLIFETIME(AValhallaSiegeWeapon, PrimarySeat);
	DOREPLIFETIME(AValhallaSiegeWeapon, SecondarySeat);
}

float AValhallaSiegeWeapon::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	// 중립 상태에선 무적?
	if (EventInstigator == nullptr /*|| TeamType == EPlayerTeamType::None*/)
	{
		return 0.f;
	}

	if (ITeamInterface* TeamInterface = Cast<ITeamInterface>(EventInstigator->GetPawn()))
	{
		// 적 팀의 데미지가 아니라면 데미지 적용 X
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

			// 공격을 가한 상대가 플레이어이거나 공성병기라면
			if (IActorTypeInterface* ActorTypeInterface = Cast<IActorTypeInterface>(InstigatorPawn))
			{
				if (ActorTypeInterface->GetActorTypeTag() == EActorType::Player || ActorTypeInterface->GetActorTypeTag() == EActorType::Vehicle)
				{
					OnVehicleHealthChanged(CurrentHealth, MaxHealth, EventInstigator);
				}
			}
		}

		if (CurrentHealth <= 0)
		{
			// 파괴되면 공격을 가한 상대방에게 골드를 줌
			// 공성병기로 파괴시켜도 골드를 줌...?
			if (IGoldInterface* InstigatorCharacter = Cast<IGoldInterface>(InstigatorPawn))
			{
				// 상대방이 미니언이 아니고 캐릭터라면 골드를 줌
				if (InstigatorCharacter->IsPlayerCharacter())
				{
					InstigatorCharacter->GainExpAndGoldInterface(0, GoldGivenAmount);
				}
			}

			DestroySiegeWeapon();
		}
	}

	return Damage;
}

void AValhallaSiegeWeapon::Destroyed()
{
	Super::Destroyed();


}

void AValhallaSiegeWeapon::OnMeshBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (HasAuthority())
	{
		ApplyAttackDamage(OtherActor);
	}
}

ESeatType AValhallaSiegeWeapon::GetSeat()
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

void AValhallaSiegeWeapon::SetVehicleSeat(AActor* InActor, const ESeatType& InSeatType)
{

}

AActor* AValhallaSiegeWeapon::GetSecondaryPawn()
{
	return nullptr;
}

USkeletalMeshComponent* AValhallaSiegeWeapon::GetVehicleMesh()
{
	return GetMesh();
}

UAISense_Sight::EVisibilityResult AValhallaSiegeWeapon::CanBeSeenFrom(const FCanBeSeenFromContext& Context, FVector& OutSeenLocation, int32& OutNumberOfLoSChecksPerformed, int32& OutNumberOfAsyncLosCheckRequested, float& OutSightStrength, int32* UserData, const FOnPendingVisibilityQueryProcessedDelegate* Delegate)
{
	AController* MyController = GetController();

	if (!IsValid(MyController))
	{
		return UAISense_Sight::EVisibilityResult::NotVisible;
	}

	FVector PawnLocation = GetMesh()->GetSocketLocation(TEXT("DetectPoint"));

	FHitResult HitResult;
	FCollisionQueryParams CollisionParams;
	CollisionParams.AddIgnoredActor(Context.IgnoreActor);

	bool bHit = GetWorld()->LineTraceSingleByChannel(HitResult, Context.ObserverLocation, PawnLocation, ECC_Visibility, CollisionParams);

	if (bHit && HitResult.GetActor() == MyController->GetPawn())
	{
		OutSeenLocation = PawnLocation;
		OutNumberOfLoSChecksPerformed = 1;
		OutNumberOfAsyncLosCheckRequested = 0;
		OutSightStrength = 1.0f;
		return UAISense_Sight::EVisibilityResult::Visible;
	}
	else
	{
		OutNumberOfLoSChecksPerformed = 1;
		OutNumberOfAsyncLosCheckRequested = 0;
		OutSightStrength = 0.0f;
		return UAISense_Sight::EVisibilityResult::NotVisible;
	}

	return UAISense_Sight::EVisibilityResult::NotVisible;
}

void AValhallaSiegeWeapon::Client_GetSubsystem_Implementation(AController* InController)
{
	if (APlayerController* NewPlayerController = Cast<APlayerController>(InController))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(NewPlayerController->GetLocalPlayer()))
		{
			Subsystem->ClearAllMappings();

			check(VehicleMappingContext);
			Subsystem->AddMappingContext(VehicleMappingContext, 0);
		}
	}
}

void AValhallaSiegeWeapon::VehicleMoveForwardTriggered(const FInputActionValue& Value)
{
	float ThrottleRate = Value.Get<float>();

	GetVehicleMovementComponent()->SetThrottleInput(ThrottleRate);
	GetVehicleMovementComponent()->SetBrakeInput(0.f);
}

void AValhallaSiegeWeapon::VehicleMoveForwardStarted(const FInputActionValue& Value)
{
	GetVehicleMovementComponent()->SetYawInput(0.f);
	GetVehicleMovementComponent()->SetThrottleInput(0.f);
}

void AValhallaSiegeWeapon::VehicleTurnRightTriggered(const FInputActionValue& Value)
{
	float TurnRate = Value.Get<float>();

	GetVehicleMovementComponent()->SetYawInput(TurnRate);
}

void AValhallaSiegeWeapon::VehicleTurnRightStarted(const FInputActionValue& Value)
{
	GetVehicleMovementComponent()->SetThrottleInput(0.f);
}

void AValhallaSiegeWeapon::VehicleTurnRightCompleted(const FInputActionValue& Value)
{
	GetVehicleMovementComponent()->SetYawInput(0.f);
}

void AValhallaSiegeWeapon::VehicleLook(const FInputActionValue& Value)
{
	FVector2D LookAxisVector = Value.Get<FVector2D>();

	AddControllerYawInput(LookAxisVector.X);
	AddControllerPitchInput(LookAxisVector.Y);
}

void AValhallaSiegeWeapon::VehicleAttack(const FInputActionValue& Value)
{
	Attack();
}

void AValhallaSiegeWeapon::GetOff(const FInputActionValue& Value)
{
	GetVehicleMovementComponent()->SetYawInput(0.f);
	GetVehicleMovementComponent()->SetThrottleInput(0.f);
	GetVehicleMovementComponent()->SetBrakeInput(0.f);

	if (AValhallaPlayerController* NewPlayerController = Cast<AValhallaPlayerController>(GetController()))
	{
		if (HasAuthority())
		{
			if (ISeatInterface* SeatInterface = Cast<ISeatInterface>(NewPlayerController->ControlledPawn))
			{
				SeatInterface->SetSeatType(ESeatType::None);
			}

			MC_GetOff(NewPlayerController->ControlledPawn);

			NewPlayerController->Possess(NewPlayerController->ControlledPawn);
			PrimarySeat = nullptr;

			TeamType = EPlayerTeamType::None;
		}
		else
		{
			Server_GetOff(NewPlayerController->ControlledPawn, NewPlayerController);
		}
	}
}

void AValhallaSiegeWeapon::Server_GetOff_Implementation(APawn* InNewPawn, AController* InController)
{
	if (ISeatInterface* SeatInterface = Cast<ISeatInterface>(InNewPawn))
	{
		SeatInterface->SetSeatType(ESeatType::None);
	}

	MC_GetOff(InNewPawn);

	InController->Possess(InNewPawn);
	PrimarySeat = nullptr;

	TeamType = EPlayerTeamType::None;

	ResetPerception();
}

void AValhallaSiegeWeapon::MC_GetOff_Implementation(APawn* InNewPawn)
{
	InNewPawn->SetActorLocation(PrimarySeatGetOffLocation->GetComponentLocation(), false, nullptr, ETeleportType::ResetPhysics);
}

EPlayerTeamType& AValhallaSiegeWeapon::GetActorTeamType()
{
	return TeamType;
}

EActorType& AValhallaSiegeWeapon::GetActorTypeTag()
{
	return ActorType;
}

void AValhallaSiegeWeapon::OnRep_CurrentHealth()
{
	if (CurrentHealth <= 0)
	{
		//Debug::Print(TEXT("SiegeWeapon Destroyed"), 5.f);
	}
}

void AValhallaSiegeWeapon::InitializeStatus()
{
	CurrentHealth = MaxHealth;
}

void AValhallaSiegeWeapon::DestroySiegeWeapon()
{
	if (PrimarySeat != nullptr)
	{
		if (AValhallaPlayerController* NewPlayerController = Cast<AValhallaPlayerController>(GetController()))
		{
			if (ISeatInterface* SeatInterface = Cast<ISeatInterface>(NewPlayerController->ControlledPawn))
			{
				SeatInterface->SetSeatType(ESeatType::None);
			}

			PrimarySeat = nullptr;
			NewPlayerController->Possess(NewPlayerController->ControlledPawn);
		}

		TeamType = EPlayerTeamType::None;
	}

	if (SecondarySeat != nullptr)
	{
		if (ISeatInterface* SeatInterface = Cast<ISeatInterface>(SecondarySeat))
		{
			SeatInterface->SetSeatType(ESeatType::None);
		}

		SecondarySeat = nullptr;
		TeamType = EPlayerTeamType::None;
	}

	Destroy();
}

void AValhallaSiegeWeapon::ApplyAttackDamage(AActor* InActor)
{
	if (IActorTypeInterface* ActorTypeInterface = Cast<IActorTypeInterface>(InActor))
	{
		if (ActorTypeInterface->GetActorTypeTag() == EActorType::Construction || ActorTypeInterface->GetActorTypeTag() == EActorType::Turret)
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

void AValhallaSiegeWeapon::CannonAttack()
{
	FVector SpawnLocation = BulletShotLocation->GetComponentLocation();
	FRotator SpawnRotator = BulletShotLocation->GetComponentRotation();
	FTransform SpawnTransform(SpawnRotator, SpawnLocation, FVector(1.f, 1.f, 1.f));

	ACannonBullet* CannonBullet = GetWorld()->SpawnActorDeferred<ACannonBullet>(CannonBulletClass, SpawnTransform, this, this, ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn);
	if (CannonBullet)
	{
		CannonBullet->SetTeamType(TeamType);
		CannonBullet->SetHitEffectScale(FVector(5.f, 5.f, 5.f));

		CannonBullet->FinishSpawning(SpawnTransform);
	}
}

void AValhallaSiegeWeapon::Server_ApplyAttackDamage_Implementation(AActor* InActor)
{
	ApplyAttackDamage(InActor);
}

void AValhallaSiegeWeapon::OnVehicleHealthChanged_Implementation(float InCurrentHealth, float InMaxHealth, AController* InInstigatorController)
{
	OnVehicleHealthChangedDelegate.ExecuteIfBound(InCurrentHealth, InMaxHealth, InInstigatorController);
}
