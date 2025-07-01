// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "SkillComponent.generated.h"

class AValhallaWeaponBase;
class AValhallaPlayerCharacter;

UCLASS(Blueprintable, BlueprintType)
class VALHALLAPROJECT_API USkillComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	USkillComponent();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

#pragma region Character Skill Data Section
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Skills)
	TArray<class UValhallaSkillBase*> CharacterSkills;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Skills)
	UDataTable* CharacterSkillDataTable = nullptr;

	AActor* TargetActor = nullptr;
#pragma endregion

#pragma region Skill Attack Section
public:
	void NormalAttack(AValhallaWeaponBase* InPlayerWeapon);
	void HardAttack(AValhallaWeaponBase* InPlayerWeapon);
	void JumpAttack(AValhallaWeaponBase* InPlayerWeapon);
	void Roaring(AValhallaWeaponBase* InPlayerWeapon);
	void UltAttack(AValhallaWeaponBase* InPlayerWeapon);

protected:
	UFUNCTION(Server, Reliable)
	void Server_AxeNormalAttack(AValhallaWeaponBase* InPlayerWeapon);

	UFUNCTION(NetMulticast, Reliable)
	void MC_AxeNormalAttack(AValhallaWeaponBase* InPlayerWeapon);

	UFUNCTION(Server, Reliable)
	void Server_AxeHardAttack(AValhallaWeaponBase* InPlayerWeapon);

	UFUNCTION(NetMulticast, Reliable)
	void MC_AxeHardAttack(AValhallaWeaponBase* InPlayerWeapon);

	UFUNCTION(Server, Reliable)
	void Server_AxeJumpAttack(AValhallaWeaponBase* InPlayerWeapon);

	UFUNCTION(NetMulticast, Reliable)
	void MC_AxeJumpAttack(AValhallaWeaponBase* InPlayerWeapon);

	UFUNCTION(Server, Reliable)
	void Server_AxeRoaring(AValhallaWeaponBase* InPlayerWeapon);

	UFUNCTION(NetMulticast, Reliable)
	void MC_AxeRoaring(AValhallaWeaponBase* InPlayerWeapon);

	UFUNCTION(Server, Reliable)
	void Server_AxeUltAttack(AValhallaWeaponBase* InPlayerWeapon);

	UFUNCTION(NetMulticast, Reliable)
	void MC_AxeUltAttack(AValhallaWeaponBase* InPlayerWeapon);
#pragma endregion

#pragma region Skill Anim Section
private:
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Skills, meta = (AllowPrivateAccess = "true"), Replicated)
	bool bIsUsingSkillNow;

public:
	FORCEINLINE void SetIsUsingSkillNow(const bool IsUsingSkillNow) { bIsUsingSkillNow = IsUsingSkillNow; }
	FORCEINLINE bool GetIsUsingSkillNow() const { return bIsUsingSkillNow; }
#pragma endregion

#pragma region Skill Input Section
public:
	void SkillInputStop();

	UFUNCTION(BlueprintCallable)
	void SkillInputStart();
#pragma endregion

#pragma region Skill CoolTime Section
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Skills, ReplicatedUsing = OnRep_CanUseHardAttackSkill)
	bool bCanUseHardAttackSkill;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Skills, ReplicatedUsing = OnRep_CanUseJumpAttackSkill)
	bool bCanUseJumpAttackSkill;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Skills, ReplicatedUsing = OnRep_CanUseRoaringSkill)
	bool bCanUseRoaringSkill;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Skills, ReplicatedUsing = OnRep_CanUseUltAttackSkill)
	bool bCanUseUltAttackSkill;

	FTimerHandle HardAttackSkillTimerHandle;
	FTimerHandle JumpAttackSkillTimerHandle;
	FTimerHandle RoaringSkillTimerHandle;
	FTimerHandle UltAttackSkillTimerHandle;

public:
	UFUNCTION()
	void OnRep_CanUseHardAttackSkill();

	UFUNCTION()
	void OnRep_CanUseJumpAttackSkill();

	UFUNCTION()
	void OnRep_CanUseRoaringSkill();

	UFUNCTION()
	void OnRep_CanUseUltAttackSkill();

	void HardAttackSkillCoolTimer();
	void JumpAttackSkillCoolTimer();
	void RoaringSkillCoolTimer();
	void UltAttackSkillCoolTimer();

	UFUNCTION(Server, Reliable)
	void Server_SetHardAttackSkillCoolTime();

	UFUNCTION(Server, Reliable)
	void Server_SetJumpAttackSkillCoolTime();

	UFUNCTION(Server, Reliable)
	void Server_SetRoaringSkillCoolTime();

	UFUNCTION(Server, Reliable)
	void Server_SetUltAttackSkillCoolTime();
#pragma endregion
};
