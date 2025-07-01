// Fill out your copyright notice in the Description page of Project Settings.


#include "Skill/ValhallaSkillBase.h"
#include "Character/ValhallaPlayerCharacter.h"
#include "Component/SkillComponent.h"
#include "Others/Structs.h"
#include "Engine/World.h"

UValhallaSkillBase::UValhallaSkillBase()
{
	CharacterSkillType = ECharacterSkillType::None;

}

void UValhallaSkillBase::ActivateSkill_Implementation(AValhallaPlayerCharacter* SkillUser, AActor* AttackTarget)
{
}

void UValhallaSkillBase::SetSkillInfo()
{
	if (SkillBaseDataTable)
	{
		UEnum* EnumPtr = StaticEnum<ECharacterSkillType>();

		if (EnumPtr)
		{
			const FString EnumNameString = EnumPtr->GetNameStringByValue(static_cast<int64>(CharacterSkillType));
			const FName CurrentSkillDataRowName = FName(*EnumNameString);
			FSkillDataTable* CurrentSkillDataRow = SkillBaseDataTable->FindRow<FSkillDataTable>(CurrentSkillDataRowName, TEXT(""));

			if (CurrentSkillDataRow)
			{
				SkillID = CurrentSkillDataRow->SkillID;
				SkillName = CurrentSkillDataRow->SkillName;
				SkillCoolTime = CurrentSkillDataRow->SkillCoolTime;
				SkillRange = CurrentSkillDataRow->SkillRange;
				SkillDamage = CurrentSkillDataRow->SkillDamage;
				SkillMana = CurrentSkillDataRow->SkillMana;
				SKillAnim = CurrentSkillDataRow->SKillAnim;
			}
		}
	}
}
