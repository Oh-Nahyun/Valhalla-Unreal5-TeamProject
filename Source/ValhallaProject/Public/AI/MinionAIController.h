// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "MinionAIController.generated.h"

/**
 * 
 */
UCLASS()
class VALHALLAPROJECT_API AMinionAIController : public AAIController
{
	GENERATED_BODY()
	
public:
	AMinionAIController();
	AMinionAIController(const FObjectInitializer& ObjectInitializer);

protected:
	virtual void BeginPlay() override;
	virtual void OnPossess(APawn* InPawn) override;
	virtual void OnUnPossess() override;
};
