// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "StatusComponent.generated.h"

class UDataTable;
class AValhallaPlayerCharacter;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class VALHALLAPROJECT_API UStatusComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UStatusComponent();

protected:
	virtual void BeginPlay() override;
	virtual void PostInitProperties() override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;


#pragma region Getter & Setter Function Section
public:
	FORCEINLINE const float GetMaxHealth() const { return MaxHealth; }
	FORCEINLINE const float GetMaxMana() const { return MaxMana; }

	FORCEINLINE const float GetHealthRegen() const { return HealthRegen; }
	FORCEINLINE const float GetManaRegen() const { return ManaRegen; }

	FORCEINLINE const float GetPhysicalAttack() const { return PhysicalAttack; }
	FORCEINLINE const float GetPhysicalDefense() const { return PhysicalDefense; }

	FORCEINLINE const float GetAttackSpeed() const { return AttackSpeed; }

	FORCEINLINE const float GetExpGain() const { return ExpGain; }
	FORCEINLINE const float GetMaxExp() const { return MaxExp; }

	FORCEINLINE const uint8 GetCurrentLevel() const { return CurrentLevel; }
#pragma endregion


#pragma region Status Section
public:
	void Initialize();

	void LevelUp();

private:
	void SetStatus(const uint8 InCharacterLevel);
#pragma endregion


#pragma region Status Variable Section
private:
	UPROPERTY(EditAnyWhere, BlueprintReadOnly, Category = "StatusData|DataTable", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UDataTable> StatusDataTable;

	UPROPERTY(EditAnyWhere, BlueprintReadOnly, Category = "StatusData|BaseStatus", meta = (AllowPrivateAccess = "true"), Replicated)
	float MaxHealth;

	UPROPERTY(EditAnyWhere, BlueprintReadOnly, Category = "StatusData|BaseStatus", meta = (AllowPrivateAccess = "true"), Replicated)
	float MaxMana;

public:
	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = "StatusData|BaseStatus", meta = (AllowPrivateAccess = "true"), Replicated)
	float HealthRegen;

	UPROPERTY(EditAnyWhere, BlueprintReadOnly, Category = "StatusData|BaseStatus", meta = (AllowPrivateAccess = "true"), Replicated)
	float ManaRegen;

public:
	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = "StatusData|CombatStatus", meta = (AllowPrivateAccess = "true"), Replicated)
	float PhysicalAttack;

	UPROPERTY(EditAnyWhere, BlueprintReadOnly, Category = "StatusData|CombatStatus", meta = (AllowPrivateAccess = "true"), Replicated)
	float MagicalAttack;

	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = "StatusData|CombatStatus", meta = (AllowPrivateAccess = "true"), Replicated)
	float PhysicalDefense;

	UPROPERTY(EditAnyWhere, BlueprintReadOnly, Category = "StatusData|CombatStatus", meta = (AllowPrivateAccess = "true"), Replicated)
	float MagicalDefense;

	UPROPERTY(EditAnyWhere, BlueprintReadOnly, Category = "StatusData|CombatStatus", meta = (AllowPrivateAccess = "true"), Replicated)
	float AttackSpeed;

private:
	UPROPERTY(EditAnyWhere, BlueprintReadOnly, Category = "StatusData|UtilStatus", meta = (AllowPrivateAccess = "true"), Replicated)
	float MoveSpeed;

	UPROPERTY(EditAnyWhere, BlueprintReadOnly, Category = "StatusData|UtilStatus", meta = (AllowPrivateAccess = "true"), Replicated)
	float Sight;

	UPROPERTY(EditAnyWhere, BlueprintReadOnly, Category = "StatusData|ExpStatus", meta = (AllowPrivateAccess = "true"), Replicated)
	float MaxExp;

	UPROPERTY(EditAnyWhere, BlueprintReadOnly, Category = "StatusData|ExpStatus", meta = (AllowPrivateAccess = "true"), Replicated)
	float ExpGain;

	UPROPERTY(EditAnyWhere, BlueprintReadOnly, Category = "StatusData|LevelStatus", meta = (AllowPrivateAccess = "true"), ReplicatedUsing = OnRep_CurrentLevel)
	uint8 CurrentLevel;

	UPROPERTY(EditAnyWhere, BlueprintReadOnly, Category = "StatusData|LevelStatus", meta = (AllowPrivateAccess = "true"), Replicated)
	uint8 MaxLevel;

public:
	UFUNCTION()
	void OnRep_CurrentLevel();
#pragma endregion


private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = OwnerCharacter, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<AValhallaPlayerCharacter> OwnerCharacter;
};
