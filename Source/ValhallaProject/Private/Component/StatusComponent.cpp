// Fill out your copyright notice in the Description page of Project Settings.


#include "Component/StatusComponent.h"
#include "Net/UnrealNetwork.h"

#include "Others/Structs.h"
#include "Character/ValhallaPlayerCharacter.h"

UStatusComponent::UStatusComponent()
{
	PrimaryComponentTick.bCanEverTick = false;

	CurrentLevel = 1;
	MaxLevel = 20;
}

void UStatusComponent::BeginPlay()
{
	Super::BeginPlay();

	OwnerCharacter = GetOwner<AValhallaPlayerCharacter>();
}

void UStatusComponent::PostInitProperties()
{
	Super::PostInitProperties();


}

void UStatusComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UStatusComponent, MaxHealth);
	DOREPLIFETIME(UStatusComponent, MaxMana);
	DOREPLIFETIME(UStatusComponent, HealthRegen);
	DOREPLIFETIME(UStatusComponent, ManaRegen);
	DOREPLIFETIME(UStatusComponent, PhysicalAttack);
	DOREPLIFETIME(UStatusComponent, MagicalAttack);
	DOREPLIFETIME(UStatusComponent, PhysicalDefense);
	DOREPLIFETIME(UStatusComponent, MagicalDefense);
	DOREPLIFETIME(UStatusComponent, AttackSpeed);
	DOREPLIFETIME(UStatusComponent, MoveSpeed);
	DOREPLIFETIME(UStatusComponent, Sight);
	DOREPLIFETIME(UStatusComponent, MaxExp);
	DOREPLIFETIME(UStatusComponent, ExpGain);
	DOREPLIFETIME(UStatusComponent, CurrentLevel);
	DOREPLIFETIME(UStatusComponent, MaxLevel);
}

void UStatusComponent::Initialize()
{
	ensure(StatusDataTable);

	SetStatus(CurrentLevel);
}

void UStatusComponent::SetStatus(const uint8 InCharacterLevel)
{
	FStatusDataTable* FoundedRow = StatusDataTable->FindRow<FStatusDataTable>(FName(FString::FromInt(InCharacterLevel)), TEXT("Cannot find DataTable"));
	if (FoundedRow) 
	{
		MaxHealth = FoundedRow->MaxHealth;
		MaxMana = FoundedRow->MaxMana;
		HealthRegen = FoundedRow->HealthRegen;
		ManaRegen = FoundedRow->ManaRegen;

		PhysicalAttack = FoundedRow->PhysicalAttack;
		MagicalAttack = FoundedRow->MagicalAttack;
		PhysicalDefense = FoundedRow->PhysicalDefense;
		MagicalDefense = FoundedRow->MagicalDefense;
		AttackSpeed = FoundedRow->AttackSpeed;

		MoveSpeed = FoundedRow->MoveSpeed;
		Sight = FoundedRow->Sight;

		MaxExp = FoundedRow->MaxExp;
		ExpGain = FoundedRow->ExpGain;
	}
}

void UStatusComponent::OnRep_CurrentLevel()
{
	if (OwnerCharacter)
	{
		OwnerCharacter->SetLevel();
	}
}

void UStatusComponent::LevelUp()
{
	if (CurrentLevel <= MaxLevel - 1)
	{
		CurrentLevel += 1;

		SetStatus(CurrentLevel);

		OwnerCharacter->PlayRankUpSound();
		OwnerCharacter->PlayRankUpEffect();
	}
}

