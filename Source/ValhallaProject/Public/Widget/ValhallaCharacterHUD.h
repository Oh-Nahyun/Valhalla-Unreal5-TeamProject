// Fill out your copyright notice in the Description page of Project Settings.


#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ValhallaCharacterHUD.generated.h"

class UImage;
class UValhallaChatWidget;
class UTexture2D;

UCLASS()
class VALHALLAPROJECT_API UValhallaCharacterHUD : public UUserWidget
{
	GENERATED_BODY()
	
public:
	virtual void NativeConstruct() override;

public:
	UPROPERTY(meta = (BindWidget))
	UImage* Image_SkillCoolTime2;

	UPROPERTY(meta = (BindWidget))
	UImage* Image_SkillCoolTime3;

	UPROPERTY(meta = (BindWidget))
	UImage* Image_SkillCoolTime4;

	UPROPERTY(meta = (BindWidget))
	UImage* Image_SkillCoolTime5;

public:
	UFUNCTION(BlueprintImplementableEvent)
	void SetHealth(float InCurrentHealth, float InMaxHealth);

	UFUNCTION(BlueprintImplementableEvent)
	void SetMana(float InCurrentMana, float InMaxMana);

	UFUNCTION(BlueprintImplementableEvent)
	void SetExp(float InCurrentExp, float InMaxExp);

	UFUNCTION(BlueprintImplementableEvent)
	void SetLevel(uint8 InNewLevel);

	UFUNCTION(BlueprintImplementableEvent)
	void SetGold(int32 InNewGold);

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void ChangeHUDArtifect(UTexture2D* InTexture2D, uint8 InNumber);

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void SetCharacterImage(uint8 InTeamType);
};
