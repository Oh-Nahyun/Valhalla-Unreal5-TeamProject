
#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "Enums.h"
#include "Structs.generated.h"

USTRUCT(Blueprintable, BlueprintType)
struct FStatusDataTable : public FTableRowBase
{
	GENERATED_BODY()

	// Base Status
	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = BaseStat)
	float MaxHealth;

	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = BaseStat)
	float MaxMana;

	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = BaseStat)
	float HealthRegen;

	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = BaseStat)
	float ManaRegen;


	// Combat Status
	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = CombatStat)
	float PhysicalAttack;

	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = CombatStat)
	float MagicalAttack;

	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = CombatStat)
	float PhysicalDefense;

	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = CombatStat)
	float MagicalDefense;

	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = CombatStat)
	float AttackSpeed;


	// Util Status
	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = UtilStat)
	float MoveSpeed;

	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = UtilStat)
	float Sight;


	// EXP Status
	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = ExpStat)
	float MaxExp;

	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = ExpStat)
	float ExpGain;
};

USTRUCT(Blueprintable, BlueprintType)
struct FWeaponStatusDataTable : public FTableRowBase
{
	GENERATED_BODY()

	// Combat Status
	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = CombatStat)
	float PhysicalAttack;

	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = CombatStat)
	float MagicalAttack;

	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = CombatStat)
	float PhysicalDefense;

	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = CombatStat)
	float MagicalDefense;

	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = CombatStat)
	float AttackSpeed;
};

USTRUCT(Blueprintable, BlueprintType)
struct FSkillDataTable : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Skill)
	FString SkillID;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Skill)
	FName SkillName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Skill)
	float SkillCoolTime;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Skill)
	float SkillRange;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Skill)
	float SkillDamage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Skill)
	float SkillMana;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Skill)
	UAnimMontage* SKillAnim;

};

USTRUCT(Blueprintable, BlueprintType)
struct FArtefactDataTable : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = Artefact)
	int32 ArtefactID;

	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = Artefact)
	FName ArtefactNameKr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Artefact)
	FName ArtefactNameEng;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Artefact)
	EArtefactType ArtefactType;

	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = Artefact)
	FText ArtefactHoldDescription;

	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = Artefact)
	FText ArtefactUseDescription;

	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = Artefact)
	float ArtefactBuyPrice;

	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = Artefact)
	float ArtefactSellPrice;

	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = Artefact)
	EArtefactHowToUse ArtefactHowToUse;

	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = Artefact)
	EArtefactEffectType ArtefactEffectType;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Artefact)
	float ArtefactPassiveEffectValue;

	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = Artefact)
	EArtefactTargetType ArtefactTargetType1;

	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = Artefact)
	EArtefactEffectType ArtefactEffectType1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Artefact)
	float ArtefactActiveEffectValue1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Artefact)
	float ArtefactActiveEffectDuration1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Artefact)
	float ArtefactCoolTime1;

	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = Artefact)
	EArtefactTargetType ArtefactTargetType2;

	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = Artefact)
	EArtefactEffectType ArtefactEffectType2;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Artefact)
	float ArtefactActiveEffectValue2;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Artefact)
	float ArtefactActiveEffectDuration2;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Artefact)
	float ArtefactCoolTime2;

	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = Artefact)
	UTexture2D* ArtefactIcon;

	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = Artefact)
	UParticleSystem* ArtefactParticle;

	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = Artefact)
	USoundBase* ArtefactSound;
};
