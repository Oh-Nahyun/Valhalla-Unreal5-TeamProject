// Fill out your copyright notice in the Description page of Project Settings.


#include "Component/InteractionComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "GameFramework/CharacterMovementComponent.h"

#include "Interface/ActorTypeInterface.h"
#include "Character/ValhallaPlayerCharacter.h"
#include "Player/ValhallaPlayerController.h"
#include "Interface/VehicleInterface.h"
#include "Sanctum/ValhallaSanctum.h"
#include "Widget/ValhallaSanctumHUD.h"
#include "Others/Enums.h"

UInteractionComponent::UInteractionComponent()
{
	PrimaryComponentTick.bCanEverTick = false;

}

void UInteractionComponent::BeginPlay()
{
	Super::BeginPlay();

	
}

void UInteractionComponent::DoInteraction(AActor* InActor)
{
	if (!InActor)
	{
		return;
	}

	if (IActorTypeInterface* ActorTypeInterface = Cast<IActorTypeInterface>(InActor))
	{
		if (ActorTypeInterface->GetActorTypeTag() == EActorType::Vehicle)
		{
			if (GetOwnerRole() == ROLE_Authority)
			{
				VehicleInteraction(InActor);
			}
			else
			{
				Server_VehicleInteraction(InActor);
			}
		}
		else if (ActorTypeInterface->GetActorTypeTag() == EActorType::Sanctum)
		{
			SanctumInteraction(InActor);
		}
	}
}

void UInteractionComponent::SanctumInteraction(AActor* InActor)
{
	AValhallaSanctum* OurSanctum = Cast<AValhallaSanctum>(InActor);
	AValhallaPlayerCharacter* Player = GetOwner<AValhallaPlayerCharacter>();
	PlayerController = Cast<AValhallaPlayerController>(Player->GetController());
	if (OurSanctum && Player && PlayerController)
	{
		/*
		if (!(OurSanctum->bIsOpenedSanctum))
		{
			OurSanctum->OpenSanctum();
			OurSanctum->bIsOpenedSanctum = true;

			if (OurSanctum->SanctumWidget)
			{
				FInputModeUIOnly InputMode;
				InputMode.SetWidgetToFocus(OurSanctum->SanctumWidget->TakeWidget());
				PlayerController->SetInputMode(InputMode);
				PlayerController->bShowMouseCursor = true;

				PlayerController->SetIgnoreMoveInput(true);
				PlayerController->SetIgnoreLookInput(true);

				OurSanctum->SanctumWidget->PlayerCharacterRef = Player;
			}
		}
		*/
		OurSanctum->OpenSanctum();

		if (OurSanctum->SanctumWidget)
		{
			FInputModeUIOnly InputMode;
			InputMode.SetWidgetToFocus(OurSanctum->SanctumWidget->TakeWidget());
			PlayerController->SetInputMode(InputMode);
			PlayerController->bShowMouseCursor = true;

			OurSanctum->SanctumWidget->PlayerCharacterRef = Player;
		}
		/*
		else
		{
			OurSanctum->CloseSanctum();
			OurSanctum->bIsOpenedSanctum = false;

			if (PlayerController)
			{
				FInputModeGameOnly InputMode;
				PlayerController->SetInputMode(InputMode);
				PlayerController->bShowMouseCursor = false;

				PlayerController->SetIgnoreMoveInput(false);
				PlayerController->SetIgnoreLookInput(false);
			}
		}
		*/
	}
}

void UInteractionComponent::VehicleInteraction(AActor* InActor)
{
	ITeamInterface* VehicleTeamInterface = Cast<ITeamInterface>(InActor);
	ITeamInterface* OwnerTeamInterface = Cast<ITeamInterface>(GetOwner());

	check(VehicleTeamInterface && OwnerTeamInterface);

	if (VehicleTeamInterface->GetActorTeamType() != EPlayerTeamType::None && VehicleTeamInterface->GetActorTeamType() != OwnerTeamInterface->GetActorTeamType())
	{
		return;
	}

	IVehicleInterface* VehicleInterface = Cast<IVehicleInterface>(InActor);
	if (!VehicleInterface || VehicleInterface->GetSeat() == ESeatType::NoSeat)
	{
		return;
	}

	GetInTheVehicle(InActor, VehicleInterface->GetSeat());
	VehicleInterface->SetVehicleSeat(GetOwner(), VehicleInterface->GetSeat());
}

void UInteractionComponent::GetInTheVehicle(AActor* InActor, const ESeatType& InSeatType)
{
	if (AValhallaPlayerCharacter* Player = GetOwner<AValhallaPlayerCharacter>())
	{
		Player->SetPlayerOnBoard(InActor, InSeatType);
		Player->OnRep_PlayerSeatType();
	}
}

void UInteractionComponent::Server_VehicleInteraction_Implementation(AActor* InActor)
{
	VehicleInteraction(InActor);
}

