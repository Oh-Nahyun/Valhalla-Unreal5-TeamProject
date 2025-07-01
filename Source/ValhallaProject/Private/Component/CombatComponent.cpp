// Fill out your copyright notice in the Description page of Project Settings.


#include "Component/CombatComponent.h"

#include "Net/UnrealNetwork.h"

#include "Character/ValhallaPlayerCharacter.h"
#include "Actors/Weapon/ValhallaWeaponAxe.h"

UCombatComponent::UCombatComponent()
{
	PrimaryComponentTick.bCanEverTick = true;

	OwningPlayer = nullptr;

	IsAttackingNow = false;
}

void UCombatComponent::BeginPlay()
{
	Super::BeginPlay();

}

void UCombatComponent::PostInitProperties()
{
	Super::PostInitProperties();

}

void UCombatComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

}

void UCombatComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UCombatComponent, IsAttackingNow);
}

AValhallaPlayerCharacter* UCombatComponent::GetOwningPlayer()
{
	if (OwningPlayer == nullptr)
	{
		OwningPlayer = GetOwner<AValhallaPlayerCharacter>();
		return OwningPlayer;
	}

	return OwningPlayer->IsValidLowLevel() ? OwningPlayer : nullptr;
}

void UCombatComponent::NormalAttack(AValhallaWeaponBase* InPlayerWeapon)
{
	if (InPlayerWeapon->GetWeaponType() == EPlayerWeaponType::Axe)
	{
		if (GetOwnerRole() == ROLE_Authority)
		{
			if (IsAttackingNow)
			{
				return;
			}

			IsAttackingNow = true;

			MC_AxeNormalAttack(InPlayerWeapon);
		}
		else
		{
			Server_AxeNormalAttack(InPlayerWeapon);
		}
	}
	else
	{
		BowNormalAttack(InPlayerWeapon);
	}
}

void UCombatComponent::AxeNormalAttack()
{
}

void UCombatComponent::Server_AxeNormalAttack_Implementation(AValhallaWeaponBase* InPlayerWeapon)
{
	if (IsAttackingNow)
	{
		return;
	}

	IsAttackingNow = true;

	MC_AxeNormalAttack(InPlayerWeapon);
}

void UCombatComponent::MC_AxeNormalAttack_Implementation(AValhallaWeaponBase* InPlayerWeapon)
{
	if (AValhallaWeaponAxe* Axe = Cast<AValhallaWeaponAxe>(InPlayerWeapon))
	{
		if (Axe->GetCurrentComboCount() >= Axe->GetMaxComboCount())
		{
			UAnimMontage* AttackAnim = Axe->GetComboAnim(Axe->GetCurrentComboCount());
			float MontagePlaySpeed = GetOwningPlayer()->GetFinalAttackSpeed();

			GetOwningPlayer()->GetMesh()->GetAnimInstance()->Montage_Play(AttackAnim, MontagePlaySpeed);

			Axe->SetCurrentComboCount(0);
		}
		else
		{
			UAnimMontage* AttackAnim = Axe->GetComboAnim(Axe->GetCurrentComboCount());
			float MontagePlaySpeed = GetOwningPlayer()->GetFinalAttackSpeed();

			GetOwningPlayer()->GetMesh()->GetAnimInstance()->Montage_Play(AttackAnim, MontagePlaySpeed);

			Axe->SetCurrentComboCount(Axe->GetCurrentComboCount() + 1);
		}
	}
}

void UCombatComponent::BowNormalAttack(AValhallaWeaponBase* InPlayerWeapon)
{

}

void UCombatComponent::OnRep_IsAttackingNow()
{
}

