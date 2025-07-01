// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Others/Enums.h"
#include "ValhallaSkillBase.generated.h"

class AValhallaPlayerCharacter;

UCLASS(Blueprintable, BlueprintType)
class VALHALLAPROJECT_API UValhallaSkillBase : public UObject
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Skill Info")
	UDataTable* SkillBaseDataTable = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Skill Info")
	ECharacterSkillType CharacterSkillType;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Skill Info")
	FString SkillID;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Skill Info")
	FName SkillName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Skill Info")
	float SkillCoolTime;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Skill Info")
	float SkillRange;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Skill Info")
	float SkillDamage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Skill Info")
	float SkillMana;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Skill Info")
	UAnimMontage* SKillAnim;

public:
	UValhallaSkillBase();

	UFUNCTION(BlueprintNativeEvent, Category = "Skill")
	void ActivateSkill(AValhallaPlayerCharacter* SkillUser, AActor* AttackTarget);
	virtual void ActivateSkill_Implementation(AValhallaPlayerCharacter* SkillUser, AActor* AttackTarget);

	UFUNCTION(BlueprintCallable, Category = "Skill")
	void SetSkillInfo();
	
};
