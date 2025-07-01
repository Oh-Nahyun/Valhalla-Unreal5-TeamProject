// Fill out your copyright notice in the Description page of Project Settings.

#include "Sanctum/ValhallaSanctum.h"
#include "Component/StatusComponent.h"
#include "Character/ValhallaPlayerCharacter.h"
#include "Player/ValhallaPlayerController.h"
#include "Blueprint/UserWidget.h"
#include "Widget/ValhallaSanctumHUD.h"
#include "Others/Debug.h"

AValhallaSanctum::AValhallaSanctum()
{
	PrimaryActorTick.bCanEverTick = true;

	SanctumMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("SanctumMesh"));
	RootComponent = SanctumMesh;

	ActorType = EActorType::Sanctum;
	bIsOpenedSanctum = false;
	bIsSanctumAlreadyMade = false;
}

void AValhallaSanctum::OpenSanctum()
{
	if (!SanctumHUD)
	{
		return;
	}

	if (bIsSanctumAlreadyMade)
	{
		SanctumWidget->SetVisibility(ESlateVisibility::Visible);
	}
	else
	{
		SanctumWidget = CreateWidget<UValhallaSanctumHUD>(GetWorld()->GetFirstPlayerController(), SanctumHUD);
		if (SanctumWidget && !SanctumWidget->IsInViewport())
		{
			SanctumWidget->AddToViewport();
			bIsSanctumAlreadyMade = true;
		}
	}
}

void AValhallaSanctum::CloseSanctum()
{
	if (SanctumWidget)
	{
		SanctumWidget->SetVisibility(ESlateVisibility::Hidden);
	}
}

EActorType& AValhallaSanctum::GetActorTypeTag()
{
	return ActorType;
}
