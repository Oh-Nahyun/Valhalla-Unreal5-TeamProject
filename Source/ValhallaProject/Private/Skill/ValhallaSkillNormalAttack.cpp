// Fill out your copyright notice in the Description page of Project Settings.


#include "Skill/ValhallaSkillNormalAttack.h"
#include "Character/ValhallaPlayerCharacter.h"

UValhallaSkillNormalAttack::UValhallaSkillNormalAttack()
{
	CharacterSkillType = ECharacterSkillType::LeftClick;

}

void UValhallaSkillNormalAttack::ActivateSkill_Implementation(AValhallaPlayerCharacter* SkillUser, AActor* AttackTarget)
{
	if (!SkillUser) return;

	Super::ActivateSkill_Implementation(SkillUser, AttackTarget);

}
