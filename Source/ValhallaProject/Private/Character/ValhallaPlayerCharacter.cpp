// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/ValhallaPlayerCharacter.h"

#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Components/BoxComponent.h"
#include "Net/UnrealNetwork.h"
#include "Components/CapsuleComponent.h"
#include "Components/WidgetComponent.h"
#include "Kismet/GameplayStatics.h"
#include "TimerManager.h"

#include "Component/ValhallaCameraComponent.h"
#include "Component/StatusComponent.h"
#include "Component/InteractionComponent.h"
#include "Skill/ValhallaSkillBase.h"
#include "Component/SkillComponent.h"
#include "Actors/Weapon/ValhallaWeaponBase.h"
#include "Data/Weapon/WeaponData.h"
#include "Player/ValhallaPlayerController.h"
#include "Interface/VehicleInterface.h"
#include "Widget/ValhallaHealthWidget.h"
#include "Widget/ValhallaCharacterHUD.h"
#include "Actors/Vehicle/Refactoring/VehicleSecondaryPawn.h"
#include "Component/ItemComponent.h"

AValhallaPlayerCharacter::AValhallaPlayerCharacter()
{
	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 500.0f, 0.0f);

	GetCharacterMovement()->GravityScale = 1.75f;
	GetCharacterMovement()->JumpZVelocity = 700.f;
	GetCharacterMovement()->AirControl = 0.35f;
	GetCharacterMovement()->MaxWalkSpeed = 500.f;
	GetCharacterMovement()->MinAnalogWalkSpeed = 20.f;
	GetCharacterMovement()->BrakingDecelerationWalking = 2000.f;
	GetCharacterMovement()->BrakingDecelerationFalling = 1500.0f;

	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->SetRelativeLocation(FVector(0.f, 0.f, 50.f));
	CameraBoom->TargetArmLength = 400.0f;
	CameraBoom->bUsePawnControlRotation = true;
	CameraBoom->bDoCollisionTest = false;

	FollowCamera = CreateDefaultSubobject<UValhallaCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	FollowCamera->bUsePawnControlRotation = false;

	StatusComponent = CreateDefaultSubobject<UStatusComponent>(TEXT("StatusComponent"));

	InteractionComponent = CreateDefaultSubobject<UInteractionComponent>(TEXT("InteractionComponent"));

	SkillComponent = CreateDefaultSubobject<USkillComponent>(TEXT("SkillComponent"));

	ItemComponent = CreateDefaultSubobject<UItemComponent>(TEXT("ItemComponent"));

	CharacterHealthWidgetComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("CharacterHealthWidgetComponent"));
	CharacterHealthWidgetComponent->SetupAttachment(RootComponent);

	TeamType = EPlayerTeamType::None;

	ActorType = EActorType::Player;

	CharacterWeapon = nullptr;

	CurrentGold = 0;
	GoldGivenAmount = 300;

	DrivingVehicle = nullptr;
	PlayerSeatType = ESeatType::None;

	IsFirstInitialize = true;

	PlayerIndex = 0;

	OriginalPhysicalAttackDamage = 0.f;
	OriginalPhysicalDefense = 0.f;
	PhysicalDamageRatio = 1.f;
	PhysicalDamageIncrease = 0.f;

	DoOnceSetTimer = false;
}

void AValhallaPlayerCharacter::BeginPlay()
{
	Super::BeginPlay();

	ensure(WeaponType);
	if (HasAuthority())
	{
		CharacterWeapon = SpawnPlayerWeapon(WeaponType);
		if (CharacterWeapon)
		{
			FAttachmentTransformRules AttachmentRule(EAttachmentRule::SnapToTarget, EAttachmentRule::SnapToTarget, EAttachmentRule::SnapToTarget, true);
			CharacterWeapon->AttachToComponent(GetMesh(), AttachmentRule, "WeaponSocket");
		}
	}

	UValhallaHealthWidget* HealthWidget = Cast<UValhallaHealthWidget>(CharacterHealthWidgetComponent->GetWidget());
	if (HealthWidget)
	{
		OnCharacterHealthChangedDelegate.BindUObject(HealthWidget, &UValhallaHealthWidget::SetHealthBar);
	}

	CreateCharacterHUD();

	if (!DoOnceSetTimer)
	{
		Regen();

		DoOnceSetTimer = true;
	}
}

void AValhallaPlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AValhallaPlayerCharacter::Move);
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &AValhallaPlayerCharacter::Look);
		EnhancedInputComponent->BindAction(InteractionAction, ETriggerEvent::Started, this, &AValhallaPlayerCharacter::Interaction);

		EnhancedInputComponent->BindAction(QSkillAction, ETriggerEvent::Started, this, &AValhallaPlayerCharacter::QSkillPressed);
		EnhancedInputComponent->BindAction(ESkillAction, ETriggerEvent::Started, this, &AValhallaPlayerCharacter::ESkillPressed);
		EnhancedInputComponent->BindAction(RSkillAction, ETriggerEvent::Started, this, &AValhallaPlayerCharacter::RSkillPressed);
		EnhancedInputComponent->BindAction(RightMouseAction, ETriggerEvent::Started, this, &AValhallaPlayerCharacter::RightMouseClicked);
		EnhancedInputComponent->BindAction(LeftMouseAction, ETriggerEvent::Started, this, &AValhallaPlayerCharacter::LeftMouseClicked);

		EnhancedInputComponent->BindAction(Keyboard1Action, ETriggerEvent::Started, this, &AValhallaPlayerCharacter::Keyboard1Pressed);
		EnhancedInputComponent->BindAction(Keyboard2Action, ETriggerEvent::Started, this, &AValhallaPlayerCharacter::Keyboard2Pressed);
		EnhancedInputComponent->BindAction(Keyboard3Action, ETriggerEvent::Started, this, &AValhallaPlayerCharacter::Keyboard3Pressed);
		EnhancedInputComponent->BindAction(Keyboard4Action, ETriggerEvent::Started, this, &AValhallaPlayerCharacter::Keyboard4Pressed);
		EnhancedInputComponent->BindAction(Keyboard5Action, ETriggerEvent::Started, this, &AValhallaPlayerCharacter::Keyboard5Pressed);

		EnhancedInputComponent->BindAction(ShiftAction, ETriggerEvent::Started, this, &AValhallaPlayerCharacter::ShiftPressed);
		EnhancedInputComponent->BindAction(ShiftAction, ETriggerEvent::Completed, this, &AValhallaPlayerCharacter::ShiftReleased);
		EnhancedInputComponent->BindAction(ShiftAction, ETriggerEvent::Canceled, this, &AValhallaPlayerCharacter::ShiftReleased);
	}
}

void AValhallaPlayerCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AValhallaPlayerCharacter::PostInitializeComponents()
{
	Super::PostInitializeComponents();

}

void AValhallaPlayerCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	SetControlledPawn(NewController);

	if (IsFirstInitialize)
	{
		StatusComponent->Initialize();
		InitializeHealthAndMana();

		IsFirstInitialize = false;
	}

	Client_GetSubsystem(NewController);

	if (CharacterWeapon)
	{
		CharacterWeapon->SetActorHiddenInGame(false);
	}

	IsCannon = false;
	BallistaRotationPitch = 0.f;
}

void AValhallaPlayerCharacter::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();


}

void AValhallaPlayerCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AValhallaPlayerCharacter, CurrentMana);
	DOREPLIFETIME(AValhallaPlayerCharacter, CurrentExp);
	DOREPLIFETIME(AValhallaPlayerCharacter, CurrentGold);

	DOREPLIFETIME(AValhallaPlayerCharacter, CharacterWeapon);

	DOREPLIFETIME(AValhallaPlayerCharacter, RespawnTransform);

	DOREPLIFETIME(AValhallaPlayerCharacter, PlayerSeatType);
	DOREPLIFETIME(AValhallaPlayerCharacter, DrivingVehicle);
	DOREPLIFETIME(AValhallaPlayerCharacter, IsCannon);

	DOREPLIFETIME(AValhallaPlayerCharacter, OriginalPhysicalAttackDamage);
	DOREPLIFETIME(AValhallaPlayerCharacter, OriginalPhysicalDefense);
	DOREPLIFETIME(AValhallaPlayerCharacter, PhysicalDamageRatio);
	DOREPLIFETIME(AValhallaPlayerCharacter, PhysicalDamageIncrease);

	DOREPLIFETIME(AValhallaPlayerCharacter, DoOnceSetTimer);
}

float AValhallaPlayerCharacter::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	if (EventInstigator == nullptr || PlayerCurrentState == EPlayerCurrentState::Dead || PlayerCurrentState == EPlayerCurrentState::OnBoard)
	{
		return 0.f;
	}

	if (ITeamInterface* TeamInterface = Cast<ITeamInterface>(EventInstigator->GetPawn()))
	{
		EPlayerTeamType Team = TeamInterface->GetActorTeamType();

		if (TeamInterface->GetActorTeamType() == TeamType)
		{
			return 0.f;
		}
	}

	Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

	if (HasAuthority())
	{
		DamagedCharacterHealth(DamageAmount);

		if (IActorTypeInterface* ActorTypeInterface = Cast<IActorTypeInterface>(EventInstigator->GetPawn<APawn>()))
		{
			if (ActorTypeInterface->GetActorTypeTag() == EActorType::Player || ActorTypeInterface->GetActorTypeTag() == EActorType::Vehicle)
			{
				OnCharacterHealthChanged(CurrentHealth, StatusComponent->GetMaxHealth(), EventInstigator);
			}
		}

		if (IsDead(CurrentHealth))
		{
			Die();
			OnRep_PlayerCurrentState();

			if (IActorTypeInterface* InstigatorCharacter = Cast<IActorTypeInterface>(EventInstigator->GetPawn()))
			{
				if (InstigatorCharacter->GetActorTypeTag() == EActorType::Player)
				{
					if (AValhallaPlayerCharacter* PlayerCharacter = Cast<AValhallaPlayerCharacter>(InstigatorCharacter))
					{
						float Exp = StatusComponent->GetExpGain();
						PlayerCharacter->GainExpAndGold(Exp, GoldGivenAmount);
					}
				}
				else if (InstigatorCharacter->GetActorTypeTag() == EActorType::Vehicle)
				{
					if (AValhallaPlayerController* InstigatorController = Cast<AValhallaPlayerController>(EventInstigator))
					{
						float Exp = StatusComponent->GetExpGain();
						InstigatorController->ControlledPlayer->GainExpAndGold(Exp, GoldGivenAmount);
					}
				}
			}
		}
	}

	return DamageAmount;
}

void AValhallaPlayerCharacter::SetControlledPawn(AController* InNewController)
{
	if (AValhallaPlayerController* NewPlayerController = Cast<AValhallaPlayerController>(InNewController))
	{
		NewPlayerController->ControlledPawn = this;
		NewPlayerController->ControlledPlayer = this;
	}
}

void AValhallaPlayerCharacter::Move(const FInputActionValue& Value)
{
	FVector2D MovementVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

		AddMovementInput(ForwardDirection, MovementVector.Y);
		AddMovementInput(RightDirection, MovementVector.X);
	}
}

void AValhallaPlayerCharacter::Look(const FInputActionValue& Value)
{
	FVector2D LookAxisVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		AddControllerYawInput(LookAxisVector.X);
		AddControllerPitchInput(LookAxisVector.Y);
	}
}

void AValhallaPlayerCharacter::Interaction()
{
	InteractionComponent->DoInteraction(FollowCamera->GetTraceHitResult());
}

void AValhallaPlayerCharacter::QSkillPressed()
{
	if (IsManaZero())
	{
		return;
	}

	if (!(SkillComponent->bCanUseJumpAttackSkill))
	{
		return;
	}

	if (SkillComponent->GetIsUsingSkillNow())
	{
		return;
	}

	if (SkillComponent->CharacterSkills[2]->SkillMana > CurrentMana)
	{
		return;
	}
	  
	if (SkillComponent->CharacterSkills[2])
	{
		ApplySkillMana(2);

		ApplySkillDamage(2);

		SkillComponent->JumpAttack(CharacterWeapon);
	}

	StartJumpAttack();
}

void AValhallaPlayerCharacter::ESkillPressed()
{
	if (IsManaZero())
	{
		return;
	}

	if (!(SkillComponent->bCanUseRoaringSkill))
	{
		return;
	}

	if (SkillComponent->GetIsUsingSkillNow())
	{
		return;
	}

	if (SkillComponent->CharacterSkills[3]->SkillMana > CurrentMana)
	{
		return;
	}

	if (SkillComponent->CharacterSkills[3])
	{
		ApplySkillMana(3);

		ApplySkillDefense();

		SkillComponent->Roaring(CharacterWeapon);
	}
}

void AValhallaPlayerCharacter::RSkillPressed()
{
	if (IsManaZero())
	{
		return;
	}

	if (!(SkillComponent->bCanUseUltAttackSkill))
	{
		return;
	}

	if (SkillComponent->GetIsUsingSkillNow())
	{
		return;
	}

	if (SkillComponent->CharacterSkills[4]->SkillMana > CurrentMana)
	{
		return;
	}

	if (SkillComponent->CharacterSkills[4])
	{
		ApplySkillMana(4);

		ApplySkillDamage(4);

		SkillComponent->UltAttack(CharacterWeapon);
	}
}

void AValhallaPlayerCharacter::RightMouseClicked()
{
	if (IsManaZero())
	{
		return;
	}

	if (!(SkillComponent->bCanUseHardAttackSkill))
	{
		return;
	}

	if (SkillComponent->GetIsUsingSkillNow())
	{
		return;
	}

	if (SkillComponent->CharacterSkills[1]->SkillMana > CurrentMana)
	{
		return;
	}

	if (SkillComponent->CharacterSkills[1])
	{
		ApplySkillMana(1);

		ApplySkillDamage(1);

		SkillComponent->HardAttack(CharacterWeapon);
	}
}

void AValhallaPlayerCharacter::LeftMouseClicked()
{
	if (SkillComponent->GetIsUsingSkillNow())
	{
		return;
	}

	if (SkillComponent->CharacterSkills[0])
	{
		ApplySkillMana(0);

		ApplySkillDamage(0);

		SkillComponent->NormalAttack(CharacterWeapon);
	}
}

void AValhallaPlayerCharacter::Keyboard1Pressed()
{
	if (!ItemComponent)
	{
		return;
	}

	if (ItemComponent->CharacterInventoryItemNames.Num() <= 0)
	{
		return;
	}

	if (!ItemComponent->bCanPressKeyboard1)
	{
		return;
	}

	ItemComponent->SelectInventoryItem(ItemComponent->CharacterInventoryItemNames[0], 0);
}

void AValhallaPlayerCharacter::Keyboard2Pressed()
{
	if (!ItemComponent)
	{
		return;
	}

	if (ItemComponent->CharacterInventoryItemNames.Num() <= 0)
	{
		return;
	}

	if (!ItemComponent->bCanPressKeyboard2)
	{
		return;
	}

	ItemComponent->SelectInventoryItem(ItemComponent->CharacterInventoryItemNames[1], 1);
}

void AValhallaPlayerCharacter::Keyboard3Pressed()
{
	if (!ItemComponent)
	{
		return;
	}

	if (ItemComponent->CharacterInventoryItemNames.Num() <= 0)
	{
		return;
	}

	if (!ItemComponent->bCanPressKeyboard3)
	{
		return;
	}

	ItemComponent->SelectInventoryItem(ItemComponent->CharacterInventoryItemNames[2], 2);
}

void AValhallaPlayerCharacter::Keyboard4Pressed()
{
	if (!ItemComponent)
	{
		return;
	}

	if (ItemComponent->CharacterInventoryItemNames.Num() <= 0)
	{
		return;
	}

	if (!ItemComponent->bCanPressKeyboard4)
	{
		return;
	}

	ItemComponent->SelectInventoryItem(ItemComponent->CharacterInventoryItemNames[3], 3);
}

void AValhallaPlayerCharacter::Keyboard5Pressed()
{
	if (!ItemComponent)
	{
		return;
	}

	if (ItemComponent->CharacterInventoryItemNames.Num() <= 0)
	{
		return;
	}

	if (!ItemComponent->bCanPressKeyboard5)
	{
		return;
	}

	ItemComponent->SelectInventoryItem(ItemComponent->CharacterInventoryItemNames[4], 4);
}

void AValhallaPlayerCharacter::ShiftPressed()
{

}

void AValhallaPlayerCharacter::ShiftReleased()
{

}

void AValhallaPlayerCharacter::Client_GetSubsystem_Implementation(AController* InNewController)
{
	if (AValhallaPlayerController* NewPlayerController = Cast<AValhallaPlayerController>(InNewController))
	{
		NewPlayerController->ControlledPawn = this;

		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(NewPlayerController->GetLocalPlayer()))
		{
			Subsystem->ClearAllMappings();

			check(DefaultMappingContext);
			Subsystem->AddMappingContext(DefaultMappingContext, 0);
		}
	}
}

AValhallaWeaponBase* AValhallaPlayerCharacter::SpawnPlayerWeapon(TSubclassOf<AValhallaWeaponBase> InWeaponClass)
{
	const FTransform SpawnTransform(GetActorLocation());
	AValhallaWeaponBase* SpawnedWeapon = GetWorld()->SpawnActorDeferred<AValhallaWeaponBase>(InWeaponClass, SpawnTransform, this, this, ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn);
	if (SpawnedWeapon)
	{
		SpawnedWeapon->Initialize();
		SpawnedWeapon->FinishSpawning(SpawnTransform);
		return SpawnedWeapon;
	}

	return nullptr;
}

void AValhallaPlayerCharacter::ToggleWeaponCollision(bool IsCollisionEnable)
{
	Super::ToggleWeaponCollision(IsCollisionEnable);

	if (IsCollisionEnable)
	{
		CharacterWeapon->GetWeaponCollision()->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	}
	else
	{
		CharacterWeapon->GetWeaponCollision()->SetCollisionEnabled(ECollisionEnabled::NoCollision);

		CharacterWeapon->ClearHitActors();
	}
}

float AValhallaPlayerCharacter::GetCharacterAttackPower()
{
	return StatusComponent->GetPhysicalAttack();
}

float AValhallaPlayerCharacter::GetCharacterAttackSpeed()
{
	float StatusAttackSpeed = StatusComponent->GetAttackSpeed();
	float WeaponAttackSpeed = CharacterWeapon->GetWeaponAttackSpeed();

	return StatusAttackSpeed * WeaponAttackSpeed;
}

void AValhallaPlayerCharacter::GainExpAndGoldInterface(const float InExp, const int32 InGold)
{
	GainExpAndGold(InExp, InGold);
}

bool AValhallaPlayerCharacter::IsPlayerCharacter()
{
	if (ActorType == EActorType::Player)
	{
		return true;
	}

	return false;
}

void AValhallaPlayerCharacter::SetSeatType(ESeatType InNewSeatType)
{
	PlayerSeatType = InNewSeatType;
}

void AValhallaPlayerCharacter::SetGetOffState()
{
	FDetachmentTransformRules DetachmentRules(EDetachmentRule::KeepWorld, false);
	DetachFromActor(DetachmentRules);

	UCapsuleComponent* Capsule = GetCapsuleComponent();

	Capsule->SetCollisionResponseToChannel(ECollisionChannel::ECC_GameTraceChannel10, ECollisionResponse::ECR_Block);
	Capsule->SetCollisionResponseToChannel(ECollisionChannel::ECC_GameTraceChannel7, ECollisionResponse::ECR_Block);
	Capsule->SetCollisionResponseToChannel(ECollisionChannel::ECC_GameTraceChannel11, ECollisionResponse::ECR_Block);
	Capsule->SetCollisionResponseToChannel(ECollisionChannel::ECC_GameTraceChannel9, ECollisionResponse::ECR_Block);

	GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Walking);

	DrivingVehicle = nullptr;

	PlayerCurrentState = EPlayerCurrentState::Alive;

	PlayerSeatType = ESeatType::None;
	OnRep_PlayerSeatType();
}

const float AValhallaPlayerCharacter::GetFinalAttackSpeed()
{
	float StatusAttackSpeed = StatusComponent->GetAttackSpeed();
	float WeaponAttackSpeed = CharacterWeapon->GetWeaponAttackSpeed();

	return StatusAttackSpeed * WeaponAttackSpeed;
}

void AValhallaPlayerCharacter::OnRep_CurrentMana()
{
	if (!IsLocallyControlled())
	{
		return;
	}

	if (CharacterHUD != nullptr)
	{
		CharacterHUD->SetMana(CurrentMana, StatusComponent->GetMaxMana());
	}
}

void AValhallaPlayerCharacter::OnRep_CurrentExp()
{
	if (CharacterHUD != nullptr)
	{
		CharacterHUD->SetExp(CurrentExp, StatusComponent->GetMaxExp());
	}
}

void AValhallaPlayerCharacter::OnRep_CurrentGold()
{
	if (CharacterHUD != nullptr)
	{
		CharacterHUD->SetGold(CurrentGold);
	}
}

void AValhallaPlayerCharacter::GainExpAndGold(const float InExp, const int32 InGold)
{
	float NewExp = CurrentExp + InExp;

	if (StatusComponent->GetMaxExp() <= NewExp)
	{
		float RemainingExp = NewExp - StatusComponent->GetMaxExp();
		CurrentExp = RemainingExp;
		StatusComponent->LevelUp();

		StatusComponent->OnRep_CurrentLevel();

		OnRep_CurrentExp();

		OnRep_CurrentHealth();
	}
	else
	{
		CurrentExp = NewExp;

		OnRep_CurrentExp();
	}

	CurrentGold += InGold;

	OnRep_CurrentGold();
}

void AValhallaPlayerCharacter::ChangedGoldAfterBuyingItem_Implementation(const int32 InBuyPrice)
{
	CurrentGold -= InBuyPrice;

	OnRep_CurrentGold();
}

void AValhallaPlayerCharacter::ChangedGoldBySellingItem_Implementation(const int32 InSellPrice)
{
	CurrentGold += InSellPrice;

	OnRep_CurrentGold();
}

void AValhallaPlayerCharacter::Die()
{
	Super::Die();

	PlayDieSound();

	PlayerCurrentState = EPlayerCurrentState::Dead;

	SetCollisionOnOff(false);

	SetInputOnOff(false);

	DieNextLogic();
}

void AValhallaPlayerCharacter::SetInputOnOff_Implementation(const bool IsInputOff)
{
	if (!IsInputOff)
	{
		APlayerController* CharacterController = Cast<APlayerController>(GetController());
		DisableInput(CharacterController);
	}
	else
	{
		APlayerController* CharacterController = Cast<APlayerController>(GetController());
		EnableInput(CharacterController);
	}
}

void AValhallaPlayerCharacter::RespawnCharacter()
{
	ResetHealthAndMana();

	SetActorTransform(RespawnTransform, false, nullptr, ETeleportType::ResetPhysics);

	SetCollisionOnOff(true);

	SetInputOnOff(true);

	PlayerCurrentState = EPlayerCurrentState::Alive;

	SkillComponent->SetIsUsingSkillNow(false);
}

void AValhallaPlayerCharacter::OnRep_PlayerSeatType()
{
	if (PlayerSeatType == ESeatType::Primary)
	{
		IVehicleInterface* VehicleInterface = Cast<IVehicleInterface>(DrivingVehicle);

		UCapsuleComponent* Capsule = GetCapsuleComponent();

		Capsule->SetCollisionResponseToChannel(ECollisionChannel::ECC_GameTraceChannel10, ECollisionResponse::ECR_Ignore);
		Capsule->SetCollisionResponseToChannel(ECollisionChannel::ECC_GameTraceChannel11, ECollisionResponse::ECR_Ignore);
		Capsule->SetCollisionResponseToChannel(ECollisionChannel::ECC_GameTraceChannel7, ECollisionResponse::ECR_Ignore);
		Capsule->SetCollisionResponseToChannel(ECollisionChannel::ECC_GameTraceChannel9, ECollisionResponse::ECR_Ignore);

		GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_None);

		FAttachmentTransformRules AttachmentRules(EAttachmentRule::SnapToTarget, EAttachmentRule::SnapToTarget, EAttachmentRule::KeepWorld, true);
		AttachToComponent(VehicleInterface->GetVehicleMesh(), AttachmentRules, TEXT("PrimarySeat"));

		if (CharacterWeapon)
		{
			CharacterWeapon->SetActorHiddenInGame(true);
		}
	}
	else if (PlayerSeatType == ESeatType::Secondary)
	{
		IVehicleInterface* VehicleInterface = Cast<IVehicleInterface>(DrivingVehicle);

		UCapsuleComponent* Capsule = GetCapsuleComponent();

		Capsule->SetCollisionResponseToChannel(ECollisionChannel::ECC_GameTraceChannel10, ECollisionResponse::ECR_Ignore);
		Capsule->SetCollisionResponseToChannel(ECollisionChannel::ECC_GameTraceChannel11, ECollisionResponse::ECR_Ignore);
		Capsule->SetCollisionResponseToChannel(ECollisionChannel::ECC_GameTraceChannel7, ECollisionResponse::ECR_Ignore);
		Capsule->SetCollisionResponseToChannel(ECollisionChannel::ECC_GameTraceChannel9, ECollisionResponse::ECR_Ignore);

		GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_None);

		if (AVehicleSecondaryPawn* ValhallaSecondaryPawn = Cast<AVehicleSecondaryPawn>(VehicleInterface->GetSecondaryPawn()))
		{
			if (IsValid(ValhallaSecondaryPawn->GetMesh()))
			{
				FAttachmentTransformRules AttachmentRules(EAttachmentRule::SnapToTarget, EAttachmentRule::SnapToTarget, EAttachmentRule::KeepWorld, true);
				AttachToComponent(ValhallaSecondaryPawn->GetMesh(), AttachmentRules, TEXT("SecondarySeat"));
			}
			else
			{
				FAttachmentTransformRules AttachmentRules(EAttachmentRule::SnapToTarget, EAttachmentRule::SnapToTarget, EAttachmentRule::KeepWorld, true);
				AttachToComponent(VehicleInterface->GetVehicleMesh(), AttachmentRules, TEXT("SecondarySeat"));
			}
		}

		if (CharacterWeapon)
		{
			CharacterWeapon->SetActorHiddenInGame(true);
		}
	}
	else if (PlayerSeatType == ESeatType::None)
	{
		UCapsuleComponent* Capsule = GetCapsuleComponent();

		FDetachmentTransformRules DetachmentRules(EDetachmentRule::KeepWorld, false);
		DetachFromActor(DetachmentRules);

		Capsule->SetCollisionResponseToChannel(ECollisionChannel::ECC_GameTraceChannel10, ECollisionResponse::ECR_Block);
		Capsule->SetCollisionResponseToChannel(ECollisionChannel::ECC_GameTraceChannel7, ECollisionResponse::ECR_Block);
		Capsule->SetCollisionResponseToChannel(ECollisionChannel::ECC_GameTraceChannel11, ECollisionResponse::ECR_Block);
		Capsule->SetCollisionResponseToChannel(ECollisionChannel::ECC_GameTraceChannel9, ECollisionResponse::ECR_Block);

		GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Walking);

		if (CharacterWeapon)
		{
			CharacterWeapon->SetActorHiddenInGame(false);
		}
	}
}

void AValhallaPlayerCharacter::SetPlayerOnBoard(AActor* InActor, const ESeatType& InSeatType)
{
	DrivingVehicle = InActor;

	PlayerCurrentState = EPlayerCurrentState::OnBoard;

	PlayerSeatType = InSeatType;

	if (PlayerSeatType == ESeatType::Primary)
	{
		if (APawn* PrimaryPawn = Cast<APawn>(DrivingVehicle))
		{
			GetController()->Possess(PrimaryPawn);
		}
	}
	else if (PlayerSeatType == ESeatType::Secondary)
	{
		if (IVehicleInterface* VehicleInterface = Cast<IVehicleInterface>(DrivingVehicle))
		{
			if (APawn* SecondaryPawn = Cast<APawn>(VehicleInterface->GetSecondaryPawn()))
			{
				GetController()->Possess(SecondaryPawn);
			}
		}
	}
}

void AValhallaPlayerCharacter::PlayBallistaShotAnim_Implementation()
{
	PlayAnimMontage(BallistaShotMontage);
}

void AValhallaPlayerCharacter::OnCharacterHealthChanged_Implementation(float InCurrentHealth, float InMaxHealth, AController* InInstigatorController)
{
	OnCharacterHealthChangedDelegate.ExecuteIfBound(InCurrentHealth, InMaxHealth, InInstigatorController);
}

void AValhallaPlayerCharacter::InitializeHealthAndMana()
{
	CurrentHealth = StatusComponent->GetMaxHealth();
	CurrentMana = StatusComponent->GetMaxMana();
	CurrentExp = 0.f;
	CurrentGold = 0;
}

void AValhallaPlayerCharacter::ResetHealthAndMana()
{
	CurrentHealth = StatusComponent->GetMaxHealth();
	CurrentMana = StatusComponent->GetMaxMana();

	OnRep_CurrentHealth();
	OnRep_CurrentMana();
}

void AValhallaPlayerCharacter::DamagedCharacterHealth(const float InDamage)
{
	float Damage = FMath::Clamp(InDamage - StatusComponent->GetPhysicalDefense(), 0, InDamage);

	float NewCurrentHealth = FMath::Clamp(CurrentHealth - Damage, 0, CurrentHealth);
	CurrentHealth = NewCurrentHealth;

	OnRep_CurrentHealth();
}

void AValhallaPlayerCharacter::CreateCharacterHUD_Implementation()
{
	if (!IsLocallyControlled())
	{
		return;
	}

	CharacterHUD = CreateWidget<UValhallaCharacterHUD>(GetWorld()->GetFirstPlayerController(), CharacterHUDClass);
	if (CharacterHUD)
	{
		CharacterHUD->SetHealth(CurrentHealth, StatusComponent->GetMaxHealth());
		CharacterHUD->SetMana(CurrentMana, StatusComponent->GetMaxMana());
		CharacterHUD->SetExp(CurrentExp, StatusComponent->GetMaxExp());
		CharacterHUD->SetLevel(StatusComponent->GetCurrentLevel());
		CharacterHUD->SetGold(CurrentGold);
		SetCharacterImage();

		CharacterHUD->AddToViewport();
	}
}

void AValhallaPlayerCharacter::OnRep_CurrentHealth()
{
	if (!IsLocallyControlled())
	{
		return;
	}

	if (CharacterHUD != nullptr)
	{
		CharacterHUD->SetHealth(CurrentHealth, StatusComponent->GetMaxHealth());
	}
}

void AValhallaPlayerCharacter::SetLevel()
{
	if (CharacterHUD != nullptr)
	{
		CharacterHUD->SetLevel(StatusComponent->GetCurrentLevel());
	}
}

void AValhallaPlayerCharacter::ApplySkillMana_Implementation(uint8 SkillNum)
{
	CurrentMana -= SkillComponent->CharacterSkills[SkillNum]->SkillMana;
	OnRep_CurrentMana();
}

void AValhallaPlayerCharacter::ApplySkillDamage_Implementation(uint8 SkillNum)
{
	OriginalPhysicalAttackDamage = StatusComponent->PhysicalAttack;
	StatusComponent->PhysicalAttack = (SkillComponent->CharacterSkills[SkillNum]->SkillDamage) * PhysicalDamageRatio + PhysicalDamageIncrease;
}

void AValhallaPlayerCharacter::ApplySkillDefense_Implementation()
{
	OriginalPhysicalDefense = StatusComponent->PhysicalDefense;
	StatusComponent->PhysicalDefense *= 1.3f;
}

void AValhallaPlayerCharacter::PlayDieSound_Implementation()
{
	if (DieSound)
	{
		UGameplayStatics::PlaySoundAtLocation(this, DieSound, GetActorLocation());
	}
}

void AValhallaPlayerCharacter::PlayRankUpSound_Implementation()
{
	if (RankUpSound)
	{
		UGameplayStatics::PlaySound2D(this, RankUpSound);
	}
}

bool AValhallaPlayerCharacter::IsManaZero()
{
	if (CurrentMana <= 0)
	{
		return true;
	}

	return false;
}

void AValhallaPlayerCharacter::StartJumpAttack_Implementation()
{
	JumpSkillRootMotionActivated();
}

void AValhallaPlayerCharacter::Regen_Implementation()
{
	GetWorld()->GetTimerManager().SetTimer(ManaRegenTimerHandle, this, &AValhallaPlayerCharacter::AddHealthAndMana, 1.f, true, 1.f);
}

void AValhallaPlayerCharacter::AddHealthAndMana_Implementation()
{
	if (CurrentMana >= StatusComponent->GetMaxMana() && CurrentHealth >= StatusComponent->GetMaxHealth())
	{
		return;
	}

	if (PlayerCurrentState == EPlayerCurrentState::Dead)
	{
		return;
	}

	if (CurrentMana < StatusComponent->GetMaxMana())
	{
		float NewCurrentMana = CurrentMana + StatusComponent->GetManaRegen();
		CurrentMana = FMath::Clamp(NewCurrentMana, 0.f, StatusComponent->GetMaxMana());

		OnRep_CurrentMana();
	}

	if (CurrentHealth < StatusComponent->GetMaxHealth())
	{
		float NewCurrentHealth = CurrentHealth + StatusComponent->GetHealthRegen();
		CurrentHealth = FMath::Clamp(NewCurrentHealth, 0.f, StatusComponent->GetMaxHealth());

		OnRep_CurrentHealth();
	}
}

void AValhallaPlayerCharacter::PlayHitSound_Implementation()
{
	if (HitSound)
	{
		UGameplayStatics::PlaySoundAtLocation(this, HitSound, GetActorLocation());
	}
}
