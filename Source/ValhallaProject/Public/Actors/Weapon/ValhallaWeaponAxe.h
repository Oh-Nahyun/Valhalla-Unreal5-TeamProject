// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Actors/Weapon/ValhallaWeaponBase.h"
#include "ValhallaWeaponAxe.generated.h"

/**
 * 
 */
UCLASS()
class VALHALLAPROJECT_API AValhallaWeaponAxe : public AValhallaWeaponBase
{
	GENERATED_BODY()
	
public:
	AValhallaWeaponAxe();

	FORCEINLINE uint8 GetCurrentComboCount() const { return CurrentComboCount; }
	FORCEINLINE uint8 GetMaxComboCount() const { return MaxComboCount; }
	FORCEINLINE UAnimMontage* GetComboAnim(const uint8 InCurrentComboCount);

	FORCEINLINE void SetCurrentComboCount(const float InNewCurrentComboCount) { CurrentComboCount = InNewCurrentComboCount; }


protected:
	virtual void PostInitProperties() override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;


#pragma region Weapon Section
private:
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Attack, meta = (AllowPrivateAccess = "true"), Replicated)
	uint8 CurrentComboCount;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Attack, meta = (AllowPrivateAccess = "true"))
	uint8 MaxComboCount;

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Attack)
	TArray<TObjectPtr<UAnimMontage>> ComboAnims;

public:
	void ComboAttack(AValhallaWeaponBase* InWeaponBase);
#pragma endregion
};
