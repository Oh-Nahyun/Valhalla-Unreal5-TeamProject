// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ValhallaSkillBase.h"
#include "ValhallaSkillHardAttack.generated.h"

UCLASS()
class VALHALLAPROJECT_API UValhallaSkillHardAttack : public UValhallaSkillBase
{
	GENERATED_BODY()

public:
	UValhallaSkillHardAttack();

	virtual void ActivateSkill_Implementation(AValhallaPlayerCharacter* SkillUser, AActor* AttackTarget) override;
	
};
