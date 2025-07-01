// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Actors/Vehicle/Refactoring/ValhallaVehicleBase.h"
#include "ValhallaVehicleCannon.generated.h"

class ACannonBullet;

/**
 * 
 */
UCLASS()
class VALHALLAPROJECT_API AValhallaVehicleCannon : public AValhallaVehicleBase
{
	GENERATED_BODY()
	
public:
	AValhallaVehicleCannon();


#pragma region Component Section
private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Component, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<USceneComponent> BulletShotLocation;
#pragma endregion


#pragma region Input Section
private:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UInputAction> CannonUpAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UInputAction> CannonDownAction;
#pragma endregion


#pragma region Attack Section
private:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Fire, meta = (AllowPrivateAccess = "true"))
	TSubclassOf<ACannonBullet> CannonBulletClass;

public:
	UFUNCTION(BlueprintCallable)
	void CannonAttack();
#pragma endregion
};
