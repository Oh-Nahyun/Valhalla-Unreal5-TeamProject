// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Skill/ValhallaSkillBase.h"
#include "ValhallaSkillRoaring.generated.h"

UCLASS()
class VALHALLAPROJECT_API UValhallaSkillRoaring : public UValhallaSkillBase
{
	GENERATED_BODY()

public:
	UValhallaSkillRoaring();

	virtual void ActivateSkill_Implementation(AValhallaPlayerCharacter* SkillUser, AActor* AttackTarget) override;
	
};
