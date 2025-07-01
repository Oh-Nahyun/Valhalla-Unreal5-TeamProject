// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "TurretAIController.generated.h"

class AValhallaTurret;

/**
 * 
 */
UCLASS()
class VALHALLAPROJECT_API ATurretAIController : public AAIController
{
	GENERATED_BODY()
	
public:
	ATurretAIController();

protected:
	virtual void BeginPlay() override;
	virtual void OnPossess(APawn* InPawn) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;


#pragma region AI Section
private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = PawnAI, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<AValhallaTurret> ControlledTurret;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = PawnAI, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UBehaviorTree> BehaviorTree;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = PawnAI, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UBlackboardData> BlackboardData;
#pragma endregion


#pragma region Target Section
public:
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = PawnAI, meta = (AllowPrivateAccess = "true"), Replicated)
	TArray<AActor*> TargetActors;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = PawnAI, meta = (AllowPrivateAccess = "true"), Replicated)
	TObjectPtr<AActor> CurrentTargetActor;

public:
	FORCEINLINE AActor* GetTargetActor() const { return CurrentTargetActor; }

	UFUNCTION(BlueprintPure)
	FORCEINLINE AActor* GetCurrentTargetActor() const { return CurrentTargetActor; }
#pragma endregion
};
