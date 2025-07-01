// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/TurretAIController.h"

#include "BehaviorTree/BehaviorTree.h"
#include "Net/UnrealNetwork.h"

#include "Actors/Turret/ValhallaTurret.h"

#include "Others/Debug.h"

ATurretAIController::ATurretAIController()
{
	TargetActors.Empty();
	CurrentTargetActor = nullptr;
}

void ATurretAIController::BeginPlay()
{
	Super::BeginPlay();

}

void ATurretAIController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	ControlledTurret = Cast<AValhallaTurret>(InPawn);

	if (BehaviorTree)
	{
		BehaviorTree->BlackboardAsset = BlackboardData;
		RunBehaviorTree(BehaviorTree);
	}
}

void ATurretAIController::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ATurretAIController, TargetActors);
	DOREPLIFETIME(ATurretAIController, CurrentTargetActor);
}

