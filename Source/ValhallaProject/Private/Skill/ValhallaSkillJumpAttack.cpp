// Fill out your copyright notice in the Description page of Project Settings.


#include "Skill/ValhallaSkillJumpAttack.h"
#include "Character/ValhallaPlayerCharacter.h"

UValhallaSkillJumpAttack::UValhallaSkillJumpAttack()
{
	CharacterSkillType = ECharacterSkillType::QKey;

}

void UValhallaSkillJumpAttack::ActivateSkill_Implementation(AValhallaPlayerCharacter* SkillUser, AActor* AttackTarget)
{
	if (!SkillUser) return;

	Super::ActivateSkill_Implementation(SkillUser, AttackTarget);

	if (SkillUser->GetMesh() && SkillUser->GetMesh()->GetAnimInstance())
	{
		SkillUser->GetMesh()->GetAnimInstance()->Montage_Play(SKillAnim);
	}

}
