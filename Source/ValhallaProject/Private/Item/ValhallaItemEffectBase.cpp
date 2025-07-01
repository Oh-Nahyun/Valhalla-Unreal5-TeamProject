// Fill out your copyright notice in the Description page of Project Settings.


#include "Item/ValhallaItemEffectBase.h"
#include "Character/ValhallaPlayerCharacter.h"
#include "Component/ItemComponent.h"
#include "Others/Structs.h"
#include "Engine/World.h"

#include "Net/UnrealNetwork.h"

UValhallaItemEffectBase::UValhallaItemEffectBase()
{
	CharacterItemType = EItemType::None;
	CharacterItemEffectType = EItemEffectType::First;
}

void UValhallaItemEffectBase::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UValhallaItemEffectBase, ItemEffectDuration);
	DOREPLIFETIME(UValhallaItemEffectBase, ItemCoolTime);
}

void UValhallaItemEffectBase::StartItemEffect_Implementation(AActor* InTargetActor)
{
}

void UValhallaItemEffectBase::EndItemEffect_Implementation(AActor* InTargetActor)
{
}

void UValhallaItemEffectBase::SetItemInfo()
{
	if (ItemBaseDataTable)
	{
		UEnum* EnumPtr = StaticEnum<EItemType>();

		if (EnumPtr)
		{
			const FString EnumNameString = EnumPtr->GetNameStringByValue(static_cast<int64>(CharacterItemType));
			const FName CurrentItemDataRowName = FName(*EnumNameString);
			FArtefactDataTable* CurrentItemDataRow = ItemBaseDataTable->FindRow<FArtefactDataTable>(CurrentItemDataRowName, TEXT(""));

			if (CurrentItemDataRow)
			{
				switch (CharacterItemEffectType)
				{
					case EItemEffectType::First:
					{
						ItemTargetType = CurrentItemDataRow->ArtefactTargetType1;
						ItemEffectType = CurrentItemDataRow->ArtefactEffectType1;
						ItemEffectValue = CurrentItemDataRow->ArtefactActiveEffectValue1;
						ItemEffectDuration = CurrentItemDataRow->ArtefactActiveEffectDuration1;
						ItemCoolTime = CurrentItemDataRow->ArtefactCoolTime1;
						break;
					}
					case EItemEffectType::Second:
					{
						ItemTargetType = CurrentItemDataRow->ArtefactTargetType2;
						ItemEffectType = CurrentItemDataRow->ArtefactEffectType2;
						ItemEffectValue = CurrentItemDataRow->ArtefactActiveEffectValue2;
						ItemEffectDuration = CurrentItemDataRow->ArtefactActiveEffectDuration2;
						ItemCoolTime = CurrentItemDataRow->ArtefactCoolTime2;
						break;
					}
				}
			}
		}
	}
}
