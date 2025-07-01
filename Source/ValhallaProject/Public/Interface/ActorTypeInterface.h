// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "Others/Enums.h"
#include "ActorTypeInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI, BlueprintType, NotBlueprintable)
class UActorTypeInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class VALHALLAPROJECT_API IActorTypeInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	UFUNCTION(BlueprintCallable)
	virtual EActorType& GetActorTypeTag() = 0;
};
