// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ValhallaItemEffectBase.h"
#include "ValhallaHornOfRagnarokFirst.generated.h"

UCLASS()
class VALHALLAPROJECT_API UValhallaHornOfRagnarokFirst : public UValhallaItemEffectBase
{
	GENERATED_BODY()

public:
	UValhallaHornOfRagnarokFirst();

	virtual void StartItemEffect_Implementation(AActor* InTargetActor) override;
	virtual void EndItemEffect_Implementation(AActor* InTargetActor) override;
};
