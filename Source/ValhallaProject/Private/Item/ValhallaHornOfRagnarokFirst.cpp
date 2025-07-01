// Fill out your copyright notice in the Description page of Project Settings.

#include "Item/ValhallaHornOfRagnarokFirst.h"
#include "Character/ValhallaPlayerCharacter.h"
#include "Component/StatusComponent.h"

UValhallaHornOfRagnarokFirst::UValhallaHornOfRagnarokFirst()
{
	CharacterItemType = EItemType::Arti_Horn;
	CharacterItemEffectType = EItemEffectType::First;
}

void UValhallaHornOfRagnarokFirst::StartItemEffect_Implementation(AActor* InTargetActor)
{
	Super::StartItemEffect_Implementation(InTargetActor);

	if (PlayerCharacter)
	{
		PlayerCharacter->PhysicalDamageIncrease += 300.f;
	}
}

void UValhallaHornOfRagnarokFirst::EndItemEffect_Implementation(AActor* InTargetActor)
{
	Super::EndItemEffect_Implementation(InTargetActor);

	if (PlayerCharacter)
	{
		PlayerCharacter->PhysicalDamageIncrease -= 300.f;
	}
}
