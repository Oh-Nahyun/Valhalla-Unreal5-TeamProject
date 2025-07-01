// Fill out your copyright notice in the Description page of Project Settings.


#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ValhallaSanctumHUD.generated.h"

class AValhallaPlayerCharacter;

UCLASS()
class VALHALLAPROJECT_API UValhallaSanctumHUD : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"), Category = "Shop")
	int32 ItemBuyPrice;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"), Category = "Shop")
	int32 ItemSellPrice;
	
public:
	UFUNCTION(BlueprintCallable, Category = "Shop")
	void BuyItem(int32 InItemBuyPrice);

	UFUNCTION(BlueprintCallable, Category = "Shop")
	void SellItem(int32 InItemSellPrice);

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "InteractionCharacter")
	AValhallaPlayerCharacter* PlayerCharacterRef;
};
