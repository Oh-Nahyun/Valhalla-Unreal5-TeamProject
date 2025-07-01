// Fill out your copyright notice in the Description page of Project Settings.


#include "Data/Weapon/WeaponData.h"

#include "Others/Enums.h"
#include "Actors/Weapon/ValhallaWeaponBase.h"

UWeaponData::UWeaponData()
{
	static ConstructorHelpers::FClassFinder<AValhallaWeaponBase> AxeClassRef(TEXT("/Script/Engine.Blueprint'/Game/_Valhalla/Prop/Weapon/BP_WeaponAxe.BP_WeaponAxe_C'"));
	if (AxeClassRef.Class)
	{
		WeaponClassArray.Emplace(AxeClassRef.Class);
	}

	static ConstructorHelpers::FClassFinder<AValhallaWeaponBase> BowClassRef(TEXT("/Script/Engine.Blueprint'/Game/_Valhalla/Prop/Weapon/BP_WeaponBow.BP_WeaponBow_C'"));
	if (BowClassRef.Class)
	{
		WeaponClassArray.Emplace(BowClassRef.Class);
	}
}

TSubclassOf<AValhallaWeaponBase> UWeaponData::GetWeaponClass(EPlayerWeaponType& InPlayerWeaponType)
{
	if (InPlayerWeaponType == EPlayerWeaponType::Axe)
	{
		return WeaponClassArray[(uint8)EPlayerWeaponType::Axe];
	}
	else
	{
		return WeaponClassArray[(uint8)EPlayerWeaponType::Bow];
	}
}
