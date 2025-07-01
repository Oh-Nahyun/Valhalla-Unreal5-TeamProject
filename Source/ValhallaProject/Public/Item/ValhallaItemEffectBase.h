// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Others/Enums.h"
#include "ValhallaItemEffectBase.generated.h"

class AValhallaPlayerCharacter;

UCLASS(Blueprintable, BlueprintType)
class VALHALLAPROJECT_API UValhallaItemEffectBase : public UObject
{
	GENERATED_BODY()

public:
	AValhallaPlayerCharacter* PlayerCharacter;

protected:
	virtual void GetLifetimeReplicatedProps(TArray< class FLifetimeProperty >& OutLifetimeProps) const override;

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item Info")
	UDataTable* ItemBaseDataTable = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item Info")
	EItemType CharacterItemType;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item Info")
	EItemEffectType CharacterItemEffectType;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item Info")
	EArtefactTargetType ItemTargetType;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item Info")
	EArtefactEffectType ItemEffectType;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item Info")
	float ItemEffectValue;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item Info", Replicated)
	float ItemEffectDuration;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item Info", Replicated)
	float ItemCoolTime;

public:
	UValhallaItemEffectBase();

	UFUNCTION(BlueprintNativeEvent, Category = "Item")
	void StartItemEffect(AActor* InTargetActor);
	virtual void StartItemEffect_Implementation(AActor* InTargetActor);

	UFUNCTION(BlueprintNativeEvent, Category = "Item")
	void EndItemEffect(AActor* InTargetActor);
	virtual void EndItemEffect_Implementation(AActor* InTargetActor);

	UFUNCTION(BlueprintCallable, Category = "Item")
	void SetItemInfo();
	
};
