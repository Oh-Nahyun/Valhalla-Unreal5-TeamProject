// Fill out your copyright notice in the Description page of Project Settings.


#include "Skill/ValhallaSkillUltAttack.h"
#include "Character/ValhallaPlayerCharacter.h"

UValhallaSkillUltAttack::UValhallaSkillUltAttack()
{
	CharacterSkillType = ECharacterSkillType::RKey;

}

void UValhallaSkillUltAttack::ActivateSkill_Implementation(AValhallaPlayerCharacter* SkillUser, AActor* AttackTarget)
{
	if (!SkillUser) return;

	Super::ActivateSkill_Implementation(SkillUser, AttackTarget);

	if (SkillUser->GetMesh() && SkillUser->GetMesh()->GetAnimInstance())
	{
		SkillUser->GetMesh()->GetAnimInstance()->Montage_Play(SKillAnim);
	}

}
