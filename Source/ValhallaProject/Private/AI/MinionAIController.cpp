// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/MinionAIController.h"

#include "Navigation/CrowdFollowingComponent.h"

AMinionAIController::AMinionAIController()
{

}

AMinionAIController::AMinionAIController(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer.SetDefaultSubobjectClass<UCrowdFollowingComponent>(TEXT("PathFollowingComponent")))
{

}

void AMinionAIController::BeginPlay()
{
	Super::BeginPlay();

	if (UCrowdFollowingComponent* CrowdFollowingComponent = FindComponentByClass<UCrowdFollowingComponent>())
	{
		CrowdFollowingComponent->SetCrowdAvoidanceQuality(ECrowdAvoidanceQuality::Good);
		CrowdFollowingComponent->SetCrowdSeparation(true);
		CrowdFollowingComponent->SetCrowdSeparationWeight(50.f);
		CrowdFollowingComponent->SetCrowdAvoidanceRangeMultiplier(1.1f);
	}
}

void AMinionAIController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);


}

void AMinionAIController::OnUnPossess()
{
	Super::OnUnPossess();

	Destroy();
}

