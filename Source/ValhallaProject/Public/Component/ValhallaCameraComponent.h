// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Camera/CameraComponent.h"
#include "ValhallaCameraComponent.generated.h"

/**
 * 
 */
UCLASS()
class VALHALLAPROJECT_API UValhallaCameraComponent : public UCameraComponent
{
	GENERATED_BODY()
	
public:
	UValhallaCameraComponent();


public:
	UFUNCTION()
	AActor* GetTraceHitResult();

private:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Trace, meta = (AllowPrivateAccess = "true"))
	float CameraTraceDistance;
};
