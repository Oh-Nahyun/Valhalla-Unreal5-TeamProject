// Fill out your copyright notice in the Description page of Project Settings.


#include "Widget/ValhallaSanctumHUD.h"
#include "Character/ValhallaPlayerCharacter.h"
#include "Player/ValhallaPlayerController.h"

void UValhallaSanctumHUD::NativeConstruct()
{
	Super::NativeConstruct();
}

void UValhallaSanctumHUD::BuyItem(int32 InItemBuyPrice)
{
	if (PlayerCharacterRef && (PlayerCharacterRef->CurrentGold) - InItemBuyPrice >= 0)
	{
		PlayerCharacterRef->ChangedGoldAfterBuyingItem(InItemBuyPrice);
	}
}

void UValhallaSanctumHUD::SellItem(int32 InItemSellPrice)
{
	if (PlayerCharacterRef)
	{
		PlayerCharacterRef->ChangedGoldBySellingItem(InItemSellPrice);
	}
}
