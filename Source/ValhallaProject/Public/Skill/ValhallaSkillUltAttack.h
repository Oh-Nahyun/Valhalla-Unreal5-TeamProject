// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Skill/ValhallaSkillBase.h"
#include "ValhallaSkillUltAttack.generated.h"

UCLASS()
class VALHALLAPROJECT_API UValhallaSkillUltAttack : public UValhallaSkillBase
{
	GENERATED_BODY()

public:
	UValhallaSkillUltAttack();

	virtual void ActivateSkill_Implementation(AValhallaPlayerCharacter* SkillUser, AActor* AttackTarget) override;
		
};
