// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ValhallaHealthWidget.generated.h"

UCLASS()
class VALHALLAPROJECT_API UValhallaHealthWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;
	
public:
	UFUNCTION(BlueprintImplementableEvent)
	void SetHealthBar(float InCurrentHealth, float InMaxHealth, AController* InPlayerController);

	UFUNCTION(BlueprintImplementableEvent)
	void SetManaBar(float InCurrentMana, float InMaxMana, AController* InPlayerController);
};
