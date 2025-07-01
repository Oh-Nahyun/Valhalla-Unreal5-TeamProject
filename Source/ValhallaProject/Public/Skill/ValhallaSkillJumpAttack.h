// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Skill/ValhallaSkillBase.h"
#include "ValhallaSkillJumpAttack.generated.h"

UCLASS()
class VALHALLAPROJECT_API UValhallaSkillJumpAttack : public UValhallaSkillBase
{
	GENERATED_BODY()

public:
	UValhallaSkillJumpAttack();

	virtual void ActivateSkill_Implementation(AValhallaPlayerCharacter* SkillUser, AActor* AttackTarget) override;
	
};
