// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ValhallaSkillBase.h"
#include "ValhallaSkillNormalAttack.generated.h"

UCLASS()
class VALHALLAPROJECT_API UValhallaSkillNormalAttack : public UValhallaSkillBase
{
	GENERATED_BODY()

public:
	UValhallaSkillNormalAttack();

	virtual void ActivateSkill_Implementation(AValhallaPlayerCharacter* SkillUser, AActor* AttackTarget) override;

};
