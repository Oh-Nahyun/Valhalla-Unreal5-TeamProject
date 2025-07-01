// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Others/Enums.h"
#include "CombatComponent.generated.h"

class AValhallaWeaponBase;
class AValhallaPlayerCharacter;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class VALHALLAPROJECT_API UCombatComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UCombatComponent();

protected:
	virtual void BeginPlay() override;
	virtual void PostInitProperties() override;

public:	
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;


#pragma region Multiplay Section
private:

#pragma endregion


#pragma region Owner Section
private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Owner, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<AValhallaPlayerCharacter> OwningPlayer;

public:
	FORCEINLINE AValhallaPlayerCharacter* GetOwningPlayer();
#pragma endregion
		

#pragma region Attack Section
public:
	void NormalAttack(AValhallaWeaponBase* InPlayerWeapon);

protected:
	void AxeNormalAttack();

	UFUNCTION(Server, Reliable)
	void Server_AxeNormalAttack(AValhallaWeaponBase* InPlayerWeapon);

	UFUNCTION(NetMulticast, Reliable)
	void MC_AxeNormalAttack(AValhallaWeaponBase* InPlayerWeapon);

	void BowNormalAttack(AValhallaWeaponBase* InPlayerWeapon);

private:
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Weapon, meta = (AllowPrivateAccess = "true"), ReplicatedUsing = OnRep_IsAttackingNow)
	uint8 IsAttackingNow : 1;

	UFUNCTION()
	void OnRep_IsAttackingNow();

public:
	FORCEINLINE void SetIsAttackingNow(const bool InAttackingNow) { IsAttackingNow = InAttackingNow; }
#pragma endregion
};
