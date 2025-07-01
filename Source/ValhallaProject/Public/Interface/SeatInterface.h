// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "Others/Enums.h"
#include "SeatInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class USeatInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class VALHALLAPROJECT_API ISeatInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	virtual void SetSeatType(ESeatType InNewSeatType) = 0;
	virtual void SetGetOffState() = 0;
};
