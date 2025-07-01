// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/ValhallaCharacterBase.h"
#include "Interface/CombatInterface.h"
#include "Interface/GoldInterface.h"
#include "Interface/SeatInterface.h"
#include "ValhallaPlayerCharacter.generated.h"

class USpringArmComponent;
class UValhallaCameraComponent;
class UInputMappingContext;
class UInputAction;
struct FInputActionValue;
class UStatusComponent;
class UInteractionComponent;
class UCombatComponent;
class USkillComponent;
class UItemComponent;
class AValhallaWeaponBase;
class UWeaponData;
class UValhallaCharacterHUD;
class USoundBase;

UCLASS()
class VALHALLAPROJECT_API AValhallaPlayerCharacter : public AValhallaCharacterBase, public ICombatInterface, public IGoldInterface, public ISeatInterface
{
	GENERATED_BODY()
	
public:
	AValhallaPlayerCharacter();

protected:
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;

protected:
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	virtual void PostInitializeComponents() override;

	virtual void PossessedBy(AController* NewController) override;
	virtual void OnRep_PlayerState() override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;


#pragma region Controller Section
private:
	void SetControlledPawn(AController* InNewController);
#pragma endregion


#pragma region Component Section
private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Components, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<USpringArmComponent> CameraBoom;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Components, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UValhallaCameraComponent> FollowCamera;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Components, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UStatusComponent> StatusComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Components, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UInteractionComponent> InteractionComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Components, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UCombatComponent> CombatComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Components, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<USkillComponent> SkillComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Components, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UItemComponent> ItemComponent;

public:
	FORCEINLINE USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	FORCEINLINE UValhallaCameraComponent* GetFollowCamera() const { return FollowCamera; }
	FORCEINLINE UStatusComponent* GetStatusComponent() const { return StatusComponent; }
	FORCEINLINE UInteractionComponent* GetInteractionComponent() const { return InteractionComponent; }
	FORCEINLINE UCombatComponent* GetCombatComponent() const { return CombatComponent; }
	FORCEINLINE USkillComponent* GetSkillComponent() const { return SkillComponent; }
	FORCEINLINE UItemComponent* GetItemComponent() const { return ItemComponent; }
#pragma endregion


#pragma region InputSection
private:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputMappingContext* DefaultMappingContext;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* MoveAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* LookAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* InteractionAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* QSkillAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* ESkillAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* RSkillAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* RightMouseAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* LeftMouseAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* Keyboard1Action;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* Keyboard2Action;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* Keyboard3Action;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* Keyboard4Action;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* Keyboard5Action;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* ShiftAction;

	void Move(const FInputActionValue& Value);
	void Look(const FInputActionValue& Value);
	void Interaction();
	void QSkillPressed();
	void ESkillPressed();
	void RSkillPressed();
	void RightMouseClicked();
	void LeftMouseClicked();
	void Keyboard1Pressed();
	void Keyboard2Pressed();
	void Keyboard3Pressed();
	void Keyboard4Pressed();
	void Keyboard5Pressed();
	void ShiftPressed();
	void ShiftReleased();

	UFUNCTION(Client, Reliable)
	void Client_GetSubsystem(AController* InNewController);
#pragma endregion


#pragma region Interface Section
public:
	virtual void ToggleWeaponCollision(bool IsCollisionEnable) override;

	virtual float GetCharacterAttackPower() override;
	virtual float GetCharacterAttackSpeed() override;

	virtual void GainExpAndGoldInterface(const float InExp, const int32 InGold) override;
	virtual bool IsPlayerCharacter() override;

	virtual void SetSeatType(ESeatType InNewSeatType) override;
	virtual void SetGetOffState() override;
#pragma endregion


#pragma region Weapon Section
private:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Weapon, meta = (AllowPrivateAccess = "true"))
	TSubclassOf<AValhallaWeaponBase> WeaponType;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Weapon, meta = (AllowPrivateAccess = "true"), Replicated)
	TObjectPtr<AValhallaWeaponBase> CharacterWeapon;

private:
	AValhallaWeaponBase* SpawnPlayerWeapon(TSubclassOf<AValhallaWeaponBase> InWeaponClass);

public:
	FORCEINLINE AValhallaWeaponBase* GetPlayerWeapon() const { return CharacterWeapon; }
	FORCEINLINE const float GetFinalAttackSpeed();
#pragma endregion


#pragma region Status Section
public:
	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = Status, Replicated)
	float OriginalPhysicalAttackDamage;

	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = Status, Replicated)
	float OriginalPhysicalDefense;

	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = Status, Replicated)
	float PhysicalDamageRatio;

	UPROPERTY(EditAnyWhere, BlueprintReadOnly, Category = Status, Replicated)
	float PhysicalDamageIncrease;

public:
	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = Status, meta = (AllowPrivateAccess = "true"), ReplicatedUsing = OnRep_CurrentMana)
	float CurrentMana;

	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = Status, meta = (AllowPrivateAccess = "true"), ReplicatedUsing = OnRep_CurrentExp)
	float CurrentExp;

	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = Status, meta = (AllowPrivateAccess = "true"), ReplicatedUsing = OnRep_CurrentGold)
	int32 CurrentGold;

	UFUNCTION()
	void OnRep_CurrentMana();

	UFUNCTION()
	void OnRep_CurrentExp();

	UFUNCTION()
	void OnRep_CurrentGold();

	virtual void InitializeHealthAndMana() override;
	virtual void ResetHealthAndMana() override;

	void DamagedCharacterHealth(const float InDamage);

	bool IsFirstInitialize;
#pragma endregion


#pragma region Gain Exp And Gold Section
public:
	void GainExpAndGold(const float InExp, const int32 InGold);

	UFUNCTION(Server, Reliable)
	void ChangedGoldAfterBuyingItem(const int32 InBuyPrice);

	UFUNCTION(Server, Reliable)
	void ChangedGoldBySellingItem(const int32 InSellPrice);
#pragma endregion


#pragma region Respawn Section
protected:
	virtual void Die() override;

public:
	UFUNCTION(BlueprintImplementableEvent)
	void DieNextLogic();

private:
	UFUNCTION(Client, Reliable)
	void SetInputOnOff(const bool IsInputOff);

	UFUNCTION(BlueprintCallable)
	void RespawnCharacter();

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Respawn, meta = (AllowPrivateAccess = "true"), Replicated)
	FTransform RespawnTransform;
#pragma endregion


#pragma region Vehicle Section
public:
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Vehicle, meta = (AllowPrivateAccess = "true"), Replicated)
	TObjectPtr<AActor> DrivingVehicle;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Vehicle, meta = (AllowPrivateAccess = "true"), ReplicatedUsing = OnRep_PlayerSeatType)
	ESeatType PlayerSeatType;

	FORCEINLINE void SetPlayerSeatType(const ESeatType InNewSeatType) { PlayerSeatType = InNewSeatType; }
	FORCEINLINE void SetDrivingVehicle(AActor* InNewDrivingVehicle) { DrivingVehicle = InNewDrivingVehicle; }

	UFUNCTION()
	void OnRep_PlayerSeatType();

	void SetPlayerOnBoard(AActor* InActor, const ESeatType& InSeatType);

	UFUNCTION(NetMulticast, Reliable, BlueprintCallable)
	void PlayBallistaShotAnim();

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Montage, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UAnimMontage> BallistaShotMontage;
#pragma endregion


#pragma region Delegate Section
private:
	UFUNCTION(NetMulticast, Reliable)
	void OnCharacterHealthChanged(float InCurrentHealth, float InMaxHealth, AController* InInstigatorController);
#pragma endregion


#pragma region Character HUD Section
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Widgets)
	TObjectPtr<UValhallaCharacterHUD> CharacterHUD;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Widgets)
	TSubclassOf<UValhallaCharacterHUD> CharacterHUDClass;

	UFUNCTION(Client, Reliable)
	void CreateCharacterHUD();

	virtual void OnRep_CurrentHealth() override;

	void SetLevel();

	UFUNCTION(BlueprintImplementableEvent)
	void SetCharacterImage();
#pragma endregion


#pragma region Player Game Start Section
private:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Index, meta = (AllowPrivateAccess = "true"))
	int32 PlayerIndex;
#pragma endregion


#pragma region Sound Section
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Sound, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<USoundBase> DieSound;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Sound, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<USoundBase> RankUpSound;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Sound, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<USoundBase> HitSound;

public:
	UFUNCTION(NetMulticast, Unreliable)
	void PlayDieSound();

	UFUNCTION(Client, Unreliable)
	void PlayRankUpSound();

	UFUNCTION(NetMulticast, Unreliable)
	void PlayHitSound();
#pragma endregion


#pragma region Effect Section
	UFUNCTION(BlueprintImplementableEvent)
	void PlayRankUpEffect();
#pragma endregion


#pragma region Skill Section
	UFUNCTION(Server, Reliable)
	void ApplySkillDamage(uint8 SkillNum);

	UFUNCTION(Server, Reliable)
	void ApplySkillDefense();

	UFUNCTION(Server, Reliable)
	void ApplySkillMana(uint8 SkillNum);

	bool IsManaZero();

	UFUNCTION(Server, Reliable)
	void StartJumpAttack();

	UFUNCTION(BlueprintImplementableEvent)
	void JumpSkillRootMotionActivated();
#pragma endregion


#pragma region Regen System Section
private:
	FTimerHandle ManaRegenTimerHandle;

	UPROPERTY(Replicated)
	bool DoOnceSetTimer;

	UFUNCTION(Client, Reliable)
	void Regen();

	UFUNCTION(Server, UnReliable)
	void AddHealthAndMana();
#pragma endregion
};
