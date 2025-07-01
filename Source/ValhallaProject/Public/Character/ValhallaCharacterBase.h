// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Others/Enums.h"
#include "Interface/WeaponInterface.h"
#include "Interface/TeamInterface.h"
#include "Interface/ActorTypeInterface.h"
#include "Interface/StateInterface.h"
#include "ValhallaCharacterBase.generated.h"

DECLARE_DELEGATE_ThreeParams(FOnCharacterHealthChangedSignature, float, float, AController*);

class UWidgetComponent;

UCLASS()
class VALHALLAPROJECT_API AValhallaCharacterBase : public ACharacter, public IWeaponInterface, public ITeamInterface, public IActorTypeInterface, public IStateInterface
{
	GENERATED_BODY()

public:
	AValhallaCharacterBase();

protected:
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;

protected:
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	virtual void PossessedBy(AController* NewController) override;
	virtual void OnRep_PlayerState() override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;


#pragma region Component Section

#pragma endregion


#pragma region Enum Section
protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Enum, meta = (AllowPrivateAccess = "true", ExposeOnSpawn = "true"), ReplicatedUsing = OnRep_TeamType)
	EPlayerTeamType TeamType;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Enum, meta = (AllowPrivateAccess = "true"))
	EActorType ActorType;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Enum, meta = (AllowPrivateAccess = "true"), ReplicatedUsing = OnRep_PlayerCurrentState)
	EPlayerCurrentState PlayerCurrentState;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Component, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UWidgetComponent> CharacterHealthWidgetComponent;

public:
	UFUNCTION()
	void OnRep_PlayerCurrentState();

	UFUNCTION()
	void OnRep_TeamType();

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void SetTeamCharacterMaterial(EPlayerTeamType InNewTeamType);

	FORCEINLINE const EPlayerTeamType& GetTeamType() const { return TeamType; }
	FORCEINLINE const EActorType& GetActorType() const { return ActorType; }
	FORCEINLINE const EPlayerCurrentState& GetPlayerCurrentState() const { return PlayerCurrentState; }

	FORCEINLINE void SetTeamType(EPlayerTeamType InPlayerTeamType) { TeamType = InPlayerTeamType; }
	FORCEINLINE void SetPlayerCurrentState(const EPlayerCurrentState InPlayerState) { PlayerCurrentState = InPlayerState; }
#pragma endregion


#pragma region Character Section
protected:
	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = Status, meta = (AllowPrivateAccess = "true"), ReplicatedUsing = OnRep_CurrentHealth)
	float CurrentHealth;

	UPROPERTY(EditAnyWhere, BlueprintReadOnly, Category = Status, meta = (AllowPrivateAccess = "true"), Replicated)
	int32 GoldGivenAmount;

public:
	UFUNCTION()
	virtual void OnRep_CurrentHealth();

	virtual void InitializeHealthAndMana();
	virtual void ResetHealthAndMana();
#pragma endregion


#pragma region Damage And Death Section
protected:
	virtual void Die();
	bool IsDead(const float InCurrentHealth);

	UFUNCTION(NetMulticast, Reliable)
	void SetCollisionOnOff(const bool IsCollisionOn);
#pragma endregion


#pragma region Montage Section
protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Montage, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UAnimMontage> DeadMontage;
#pragma endregion


#pragma region Interface Section
public:
	UFUNCTION(BlueprintCallable)
	virtual EActorType& GetActorTypeTag() override;

	virtual EPlayerTeamType& GetActorTeamType() override;
	virtual void ToggleWeaponCollision(bool IsCollisionEnable) override;

	virtual EPlayerCurrentState GetPlayerState() override { return PlayerCurrentState; };
#pragma endregion


#pragma region Delegate Section
	protected:
		FOnCharacterHealthChangedSignature OnCharacterHealthChangedDelegate;
#pragma endregion


public:
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Vehicle, meta = (AllowPrivateAccess = "true"), Replicated)
	bool IsCannon;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Vehicle, meta = (AllowPrivateAccess = "true"), Replicated)
	float BallistaRotationPitch;
};
