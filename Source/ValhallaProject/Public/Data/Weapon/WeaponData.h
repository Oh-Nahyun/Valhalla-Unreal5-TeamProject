// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "Others/Enums.h"
#include "WeaponData.generated.h"

class AValhallaWeaponBase;

/**
 * 
 */
UCLASS()
class VALHALLAPROJECT_API UWeaponData : public UDataAsset
{
	GENERATED_BODY()
	
public:
	UWeaponData();

	TSubclassOf<AValhallaWeaponBase> GetWeaponClass(EPlayerWeaponType& InPlayerWeaponType);

	UPROPERTY(EditAnyWhere, BlueprintReadOnly, Category = Weapon)
	TArray<TSubclassOf<AValhallaWeaponBase>> WeaponClassArray;
};
