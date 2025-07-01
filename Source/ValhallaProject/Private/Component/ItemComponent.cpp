// Fill out your copyright notice in the Description page of Project Settings.


#include "Component/ItemComponent.h"

#include "Character/ValhallaPlayerCharacter.h"
#include "Item/ValhallaItemEffectBase.h"
#include "Item/ValhallaHornOfRagnarokFirst.h"
#include "TimerManager.h"

#include "Net/UnrealNetwork.h"

UItemComponent::UItemComponent()
{
	PrimaryComponentTick.bCanEverTick = true;

	PlayerCharacter = GetOwner<AValhallaPlayerCharacter>();
}

void UItemComponent::BeginPlay()
{
	Super::BeginPlay();

#pragma region Item Info Setting Section
	UValhallaHornOfRagnarokFirst* HornOfRagnarokFirst = NewObject<UValhallaHornOfRagnarokFirst>(this);
	if (HornOfRagnarokFirst)
	{
		HornOfRagnarokFirst->PlayerCharacter = PlayerCharacter;
		HornOfRagnarokFirst->ItemBaseDataTable = CharacterItemDataTable;
		HornOfRagnarokFirst->SetItemInfo();
		HornOfRagnarokEffects.Add(HornOfRagnarokFirst);
	}
#pragma endregion

	if (GetOwnerRole() == ROLE_Authority)
	{
		bCanPressKeyboard1 = false;
		bCanPressKeyboard2 = false;
		bCanPressKeyboard3 = false;
		bCanPressKeyboard4 = false;
		bCanPressKeyboard5 = false;

		bIsPressKeyboard1 = false;
		bIsPressKeyboard2 = false;
		bIsPressKeyboard3 = false;
		bIsPressKeyboard4 = false;
		bIsPressKeyboard5 = false;
	}
}

void UItemComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UItemComponent, bCanPressKeyboard1);
	DOREPLIFETIME(UItemComponent, bCanPressKeyboard2);
	DOREPLIFETIME(UItemComponent, bCanPressKeyboard3);
	DOREPLIFETIME(UItemComponent, bCanPressKeyboard4);
	DOREPLIFETIME(UItemComponent, bCanPressKeyboard5);

	DOREPLIFETIME(UItemComponent, bIsPressKeyboard1);
	DOREPLIFETIME(UItemComponent, bIsPressKeyboard2);
	DOREPLIFETIME(UItemComponent, bIsPressKeyboard3);
	DOREPLIFETIME(UItemComponent, bIsPressKeyboard4);
	DOREPLIFETIME(UItemComponent, bIsPressKeyboard5);
}

void UItemComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

}

#pragma region Select Inventory Item Section
void UItemComponent::SelectInventoryItem(FName InItemName, uint8 InInventoryNum)
{
	if (InItemName == "Arti_Horn")
	{
		HornOfRagnarok(InInventoryNum);
	}
	else if (InItemName == "Arti_Beer")
	{
		BerserkerBeer(InInventoryNum);
	}
	else if (InItemName == "Arti_Grace")
	{
		GraceOfValkyrie(InInventoryNum);
	}
	else
	{
		return;
	}
}
#pragma endregion

void UItemComponent::HornOfRagnarokDurationAndCoolTimer()
{
	GetWorld()->GetTimerManager().SetTimer(HornOfRagnarokDurationTimerHandle, this, &UItemComponent::Server_SetHornOfRagnarokDurationTime, HornOfRagnarokEffects[0]->ItemEffectDuration, false);
	GetWorld()->GetTimerManager().SetTimer(HornOfRagnarokCoolTimerHandle, this, &UItemComponent::Server_SetHornOfRagnarokCoolTime, HornOfRagnarokEffects[0]->ItemEffectDuration + HornOfRagnarokEffects[0]->ItemCoolTime, false);
}

void UItemComponent::Server_SetHornOfRagnarokDurationTime_Implementation()
{
	HornOfRagnarokEffects[0]->EndItemEffect(TargetActor);
}

void UItemComponent::Server_SetHornOfRagnarokCoolTime_Implementation()
{
	if (bIsPressKeyboard1)
	{
		bCanPressKeyboard1 = true;
		bIsPressKeyboard1 = false;
	}
	else if (bIsPressKeyboard2)
	{
		bCanPressKeyboard2 = true;
		bIsPressKeyboard2 = false;
	}
	else if (bIsPressKeyboard3)
	{
		bCanPressKeyboard3 = true;
		bIsPressKeyboard3 = false;
	}
	else if (bIsPressKeyboard4)
	{
		bCanPressKeyboard4 = true;
		bIsPressKeyboard4 = false;
	}
	else if (bIsPressKeyboard5)
	{
		bCanPressKeyboard5 = true;
		bIsPressKeyboard5 = false;
	}
}

#pragma region Horn Of Ragnarok Item Section
void UItemComponent::HornOfRagnarok(uint8 InInventoryNum)
{
	if (GetOwnerRole() == ROLE_Authority)
	{
		switch (InInventoryNum)
		{
		case 0:
			bCanPressKeyboard1 = false;
			bIsPressKeyboard1 = true;
			break;
		case 1:
			bCanPressKeyboard2 = false;
			bIsPressKeyboard2 = true;
			break;
		case 2:
			bCanPressKeyboard3 = false;
			bIsPressKeyboard3 = true;
			break;
		case 3:
			bCanPressKeyboard4 = false;
			bIsPressKeyboard4 = true;
			break;
		case 4:
			bCanPressKeyboard5 = false;
			bIsPressKeyboard5 = true;
			break;
		}

		HornOfRagnarokEffects[0]->StartItemEffect(TargetActor);
		HornOfRagnarokDurationAndCoolTimer();
	}
	else
	{
		Server_HornOfRagnarok(TargetActor, InInventoryNum);
		HornOfRagnarokDurationAndCoolTimer();
	}
}

void UItemComponent::Server_HornOfRagnarok_Implementation(AActor* InTargetActor, uint8 InInventoryNum)
{
	switch (InInventoryNum)
	{
	case 0:
		bCanPressKeyboard1 = false;
		bIsPressKeyboard1 = true;
		break;
	case 1:
		bCanPressKeyboard2 = false;
		bIsPressKeyboard2 = true;
		break;
	case 2:
		bCanPressKeyboard3 = false;
		bIsPressKeyboard3 = true;
		break;
	case 3:
		bCanPressKeyboard4 = false;
		bIsPressKeyboard4 = true;
		break;
	case 4:
		bCanPressKeyboard5 = false;
		bIsPressKeyboard5 = true;
		break;
	}

	HornOfRagnarokEffects[0]->StartItemEffect(TargetActor);
}
#pragma endregion

#pragma region Berserkers Beer Item Section
void UItemComponent::BerserkerBeer(uint8 InInventoryNum)
{
	if (GetOwnerRole() == ROLE_Authority)
	{
		
	}
	else
	{
		Server_BerserkerBeer(TargetActor, InInventoryNum);
	}
}

void UItemComponent::Server_BerserkerBeer_Implementation(AActor* InTargetActor, uint8 InInventoryNum)
{
	
}
#pragma endregion

#pragma region Grace Of Valkyrie Item Section
void UItemComponent::GraceOfValkyrie(uint8 InInventoryNum)
{
	if (GetOwnerRole() == ROLE_Authority)
	{
		
	}
	else
	{
		Server_GraceOfValkyrie(TargetActor, InInventoryNum);
	}
}

void UItemComponent::Server_GraceOfValkyrie_Implementation(AActor* InTargetActor, uint8 InInventoryNum)
{
	
}
#pragma endregion

