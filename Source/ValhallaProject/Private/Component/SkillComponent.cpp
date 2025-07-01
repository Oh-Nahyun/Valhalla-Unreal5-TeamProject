// Fill out your copyright notice in the Description page of Project Settings.


#include "Component/SkillComponent.h"

#include "Character/ValhallaPlayerCharacter.h"
#include "Player/ValhallaPlayerController.h"
#include "Actors/Weapon/ValhallaWeaponAxe.h"

#include "Skill/ValhallaSkillBase.h"
#include "Skill/ValhallaSkillNormalAttack.h"
#include "Skill/ValhallaSkillHardAttack.h"
#include "Skill/ValhallaSkillJumpAttack.h"
#include "Skill/ValhallaSkillRoaring.h"
#include "Skill/ValhallaSkillUltAttack.h"
#include "TimerManager.h"

#include "Widget/ValhallaCharacterHUD.h"
#include "Components/Image.h"
#include "Components/SlateWrapperTypes.h"

#include "Net/UnrealNetwork.h"

USkillComponent::USkillComponent()
{
	PrimaryComponentTick.bCanEverTick = true;

	bIsUsingSkillNow = false;
}

void USkillComponent::BeginPlay()
{
	Super::BeginPlay();

#pragma region Skill Setting Section
	UValhallaSkillNormalAttack* NormalAttack = NewObject<UValhallaSkillNormalAttack>(this);
	if (NormalAttack)
	{
		NormalAttack->SkillBaseDataTable = CharacterSkillDataTable;
		NormalAttack->SetSkillInfo();
		CharacterSkills.Add(NormalAttack);
	}

	UValhallaSkillHardAttack* HardAttack = NewObject<UValhallaSkillHardAttack>(this);
	if (HardAttack)
	{
		HardAttack->SkillBaseDataTable = CharacterSkillDataTable;
		HardAttack->SetSkillInfo();
		CharacterSkills.Add(HardAttack);
	}

	UValhallaSkillJumpAttack* JumpAttack = NewObject<UValhallaSkillJumpAttack>(this);
	if (JumpAttack)
	{
		JumpAttack->SkillBaseDataTable = CharacterSkillDataTable;
		JumpAttack->SetSkillInfo();
		CharacterSkills.Add(JumpAttack);
	}

	UValhallaSkillRoaring* Roaring = NewObject<UValhallaSkillRoaring>(this);
	if (Roaring)
	{
		Roaring->SkillBaseDataTable = CharacterSkillDataTable;
		Roaring->SetSkillInfo();
		CharacterSkills.Add(Roaring);
	}

	UValhallaSkillUltAttack* UltAttack = NewObject<UValhallaSkillUltAttack>(this);
	if (UltAttack)
	{
		UltAttack->SkillBaseDataTable = CharacterSkillDataTable;
		UltAttack->SetSkillInfo();
		CharacterSkills.Add(UltAttack);
	}
#pragma endregion

#pragma region Skill CoolTime Section
	bCanUseHardAttackSkill = true;
	bCanUseJumpAttackSkill = true;
	bCanUseRoaringSkill = true;
	bCanUseUltAttackSkill = true;
#pragma endregion

}

void USkillComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

}

void USkillComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(USkillComponent, bIsUsingSkillNow);
	DOREPLIFETIME(USkillComponent, bCanUseHardAttackSkill);
	DOREPLIFETIME(USkillComponent, bCanUseJumpAttackSkill);
	DOREPLIFETIME(USkillComponent, bCanUseRoaringSkill);
	DOREPLIFETIME(USkillComponent, bCanUseUltAttackSkill);
}

#pragma region Skill Input Section
void USkillComponent::SkillInputStop()
{
	if (AValhallaPlayerCharacter* Player = GetOwner<AValhallaPlayerCharacter>())
	{
		if (AValhallaPlayerController* PlayerController = Cast<AValhallaPlayerController>(Player->GetController()))
		{
			PlayerController->SetIgnoreMoveInput(true);
			PlayerController->SetIgnoreLookInput(true);
		}
	}
}

void USkillComponent::SkillInputStart()
{
	if (AValhallaPlayerCharacter* Player = GetOwner<AValhallaPlayerCharacter>())
	{
		if (AValhallaPlayerController* PlayerController = Cast<AValhallaPlayerController>(Player->GetController()))
		{
			PlayerController->SetIgnoreMoveInput(false);
			PlayerController->SetIgnoreLookInput(false);
		}
	}
}
#pragma endregion

#pragma region Skill CoolTime Section
void USkillComponent::OnRep_CanUseHardAttackSkill()
{
	AValhallaPlayerCharacter* Player = GetOwner<AValhallaPlayerCharacter>();

	if (Player->IsLocallyControlled())
	{
		if (Player->CharacterHUD && Player->CharacterHUD->Image_SkillCoolTime2)
		{
			if (bCanUseHardAttackSkill)
			{
				Player->CharacterHUD->Image_SkillCoolTime2->SetVisibility(ESlateVisibility::Hidden);
			}
			else
			{
				Player->CharacterHUD->Image_SkillCoolTime2->SetVisibility(ESlateVisibility::Visible);
			}
		}
	}
}

void USkillComponent::OnRep_CanUseJumpAttackSkill()
{
	AValhallaPlayerCharacter* Player = GetOwner<AValhallaPlayerCharacter>();

	if (Player->IsLocallyControlled())
	{
		if (Player->CharacterHUD && Player->CharacterHUD->Image_SkillCoolTime3)
		{
			if (bCanUseJumpAttackSkill)
			{
				Player->CharacterHUD->Image_SkillCoolTime3->SetVisibility(ESlateVisibility::Hidden);
			}
			else
			{
				Player->CharacterHUD->Image_SkillCoolTime3->SetVisibility(ESlateVisibility::Visible);
			}
		}
	}
}

void USkillComponent::OnRep_CanUseRoaringSkill()
{
	AValhallaPlayerCharacter* Player = GetOwner<AValhallaPlayerCharacter>();

	if (Player->IsLocallyControlled())
	{
		if (Player->CharacterHUD && Player->CharacterHUD->Image_SkillCoolTime4)
		{
			if (bCanUseRoaringSkill)
			{
				Player->CharacterHUD->Image_SkillCoolTime4->SetVisibility(ESlateVisibility::Hidden);
			}
			else
			{
				Player->CharacterHUD->Image_SkillCoolTime4->SetVisibility(ESlateVisibility::Visible);
			}
		}
	}
}

void USkillComponent::OnRep_CanUseUltAttackSkill()
{
	AValhallaPlayerCharacter* Player = GetOwner<AValhallaPlayerCharacter>();

	if (Player->IsLocallyControlled())
	{
		if (Player->CharacterHUD && Player->CharacterHUD->Image_SkillCoolTime5)
		{
			if (bCanUseUltAttackSkill)
			{
				Player->CharacterHUD->Image_SkillCoolTime5->SetVisibility(ESlateVisibility::Hidden);
			}
			else
			{
				Player->CharacterHUD->Image_SkillCoolTime5->SetVisibility(ESlateVisibility::Visible);
			}
		}
	}
}

void USkillComponent::HardAttackSkillCoolTimer()
{
	GetWorld()->GetTimerManager().SetTimer(HardAttackSkillTimerHandle, this, &USkillComponent::Server_SetHardAttackSkillCoolTime, CharacterSkills[1]->SkillCoolTime, false);
}

void USkillComponent::JumpAttackSkillCoolTimer()
{
	GetWorld()->GetTimerManager().SetTimer(JumpAttackSkillTimerHandle, this, &USkillComponent::Server_SetJumpAttackSkillCoolTime, CharacterSkills[2]->SkillCoolTime, false);
}

void USkillComponent::RoaringSkillCoolTimer()
{
	GetWorld()->GetTimerManager().SetTimer(RoaringSkillTimerHandle, this, &USkillComponent::Server_SetRoaringSkillCoolTime, CharacterSkills[3]->SkillCoolTime, false);
}

void USkillComponent::UltAttackSkillCoolTimer()
{
	GetWorld()->GetTimerManager().SetTimer(UltAttackSkillTimerHandle, this, &USkillComponent::Server_SetUltAttackSkillCoolTime, CharacterSkills[4]->SkillCoolTime, false);
}

void USkillComponent::Server_SetHardAttackSkillCoolTime_Implementation()
{
	bCanUseHardAttackSkill = true;
	OnRep_CanUseHardAttackSkill();
}

void USkillComponent::Server_SetJumpAttackSkillCoolTime_Implementation()
{
	bCanUseJumpAttackSkill = true;
	OnRep_CanUseJumpAttackSkill();
}

void USkillComponent::Server_SetRoaringSkillCoolTime_Implementation()
{
	bCanUseRoaringSkill = true;
	OnRep_CanUseRoaringSkill();
}

void USkillComponent::Server_SetUltAttackSkillCoolTime_Implementation()
{
	bCanUseUltAttackSkill = true;
	OnRep_CanUseUltAttackSkill();
}
#pragma endregion

#pragma region Normal Attack Skill Section
void USkillComponent::NormalAttack(AValhallaWeaponBase* InPlayerWeapon)
{
	if (InPlayerWeapon->GetWeaponType() == EPlayerWeaponType::Axe)
	{
		if (GetOwnerRole() == ROLE_Authority)
		{
			if (bIsUsingSkillNow) return;
			bIsUsingSkillNow = true;
			
			MC_AxeNormalAttack(InPlayerWeapon);
		}
		else
		{
			Server_AxeNormalAttack(InPlayerWeapon);
		}
	}
}

void USkillComponent::Server_AxeNormalAttack_Implementation(AValhallaWeaponBase* InPlayerWeapon)
{
	if (bIsUsingSkillNow) return;
	bIsUsingSkillNow = true;

	MC_AxeNormalAttack(InPlayerWeapon);
}

void USkillComponent::MC_AxeNormalAttack_Implementation(AValhallaWeaponBase* InPlayerWeapon)
{
	if (AValhallaWeaponAxe* Axe = Cast<AValhallaWeaponAxe>(InPlayerWeapon))
	{
		AValhallaPlayerCharacter* OwningPlayer = GetOwner<AValhallaPlayerCharacter>();
		UAnimMontage* AttackAnim = Axe->GetComboAnim(Axe->GetCurrentComboCount());
		float MontagePlaySpeed = OwningPlayer->GetFinalAttackSpeed();

		OwningPlayer->GetMesh()->GetAnimInstance()->Montage_Play(AttackAnim, MontagePlaySpeed);

		if (Axe->GetCurrentComboCount() >= Axe->GetMaxComboCount())
		{
			Axe->SetCurrentComboCount(0);
		}
		else
		{
			Axe->SetCurrentComboCount(Axe->GetCurrentComboCount() + 1);
		}
	}
}
#pragma endregion

#pragma region Hard Attack Skill Section
void USkillComponent::HardAttack(AValhallaWeaponBase* InPlayerWeapon)
{
	if (InPlayerWeapon->GetWeaponType() == EPlayerWeaponType::Axe)
	{
		if (GetOwnerRole() == ROLE_Authority)
		{
			if (bIsUsingSkillNow) return;
			bIsUsingSkillNow = true;
			bCanUseHardAttackSkill = false;
			OnRep_CanUseHardAttackSkill();

			SkillInputStop();
			MC_AxeHardAttack(InPlayerWeapon);
			HardAttackSkillCoolTimer();
		}
		else
		{
			SkillInputStop();
			Server_AxeHardAttack(InPlayerWeapon);
			HardAttackSkillCoolTimer();
		}
	}
}

void USkillComponent::Server_AxeHardAttack_Implementation(AValhallaWeaponBase* InPlayerWeapon)
{
	if (bIsUsingSkillNow) return;
	bIsUsingSkillNow = true;
	bCanUseHardAttackSkill = false;
	OnRep_CanUseHardAttackSkill();

	MC_AxeHardAttack(InPlayerWeapon);
}

void USkillComponent::MC_AxeHardAttack_Implementation(AValhallaWeaponBase* InPlayerWeapon)
{
	if (AValhallaWeaponAxe* Axe = Cast<AValhallaWeaponAxe>(InPlayerWeapon))
	{
		CharacterSkills[1]->ActivateSkill(GetOwner<AValhallaPlayerCharacter>(), TargetActor);
	}
}
#pragma endregion

#pragma region Jump Attack Skill Section
void USkillComponent::JumpAttack(AValhallaWeaponBase* InPlayerWeapon)
{
	if (InPlayerWeapon->GetWeaponType() == EPlayerWeaponType::Axe)
	{
		if (GetOwnerRole() == ROLE_Authority)
		{
			if (bIsUsingSkillNow) return;
			bIsUsingSkillNow = true;
			bCanUseJumpAttackSkill = false;
			OnRep_CanUseJumpAttackSkill();

			SkillInputStop();
			MC_AxeJumpAttack(InPlayerWeapon);
			JumpAttackSkillCoolTimer();
		}
		else
		{
			SkillInputStop();
			Server_AxeJumpAttack(InPlayerWeapon);
			JumpAttackSkillCoolTimer();
		}
	}
}

void USkillComponent::Server_AxeJumpAttack_Implementation(AValhallaWeaponBase* InPlayerWeapon)
{
	if (bIsUsingSkillNow) return;
	bIsUsingSkillNow = true;
	bCanUseJumpAttackSkill = false;
	OnRep_CanUseJumpAttackSkill();

	MC_AxeJumpAttack(InPlayerWeapon);
}

void USkillComponent::MC_AxeJumpAttack_Implementation(AValhallaWeaponBase* InPlayerWeapon)
{
	if (AValhallaWeaponAxe* Axe = Cast<AValhallaWeaponAxe>(InPlayerWeapon))
	{
		CharacterSkills[2]->ActivateSkill(GetOwner<AValhallaPlayerCharacter>(), TargetActor);
	}
}
#pragma endregion

#pragma region Roaring Skill Section
void USkillComponent::Roaring(AValhallaWeaponBase* InPlayerWeapon)
{
	if (InPlayerWeapon->GetWeaponType() == EPlayerWeaponType::Axe)
	{
		if (GetOwnerRole() == ROLE_Authority)
		{
			if (bIsUsingSkillNow) return;
			bIsUsingSkillNow = true;
			bCanUseRoaringSkill = false;
			OnRep_CanUseRoaringSkill();

			MC_AxeRoaring(InPlayerWeapon);
			RoaringSkillCoolTimer();
		}
		else
		{
			Server_AxeRoaring(InPlayerWeapon);
			RoaringSkillCoolTimer();
		}
	}
}

void USkillComponent::Server_AxeRoaring_Implementation(AValhallaWeaponBase* InPlayerWeapon)
{
	if (bIsUsingSkillNow) return;
	bIsUsingSkillNow = true;
	bCanUseRoaringSkill = false;
	OnRep_CanUseRoaringSkill();

	MC_AxeRoaring(InPlayerWeapon);
}

void USkillComponent::MC_AxeRoaring_Implementation(AValhallaWeaponBase* InPlayerWeapon)
{
	if (AValhallaWeaponAxe* Axe = Cast<AValhallaWeaponAxe>(InPlayerWeapon))
	{
		CharacterSkills[3]->ActivateSkill(GetOwner<AValhallaPlayerCharacter>(), TargetActor);
	}
}
#pragma endregion

#pragma region Ult Attack Skill Section
void USkillComponent::UltAttack(AValhallaWeaponBase* InPlayerWeapon)
{
	if (InPlayerWeapon->GetWeaponType() == EPlayerWeaponType::Axe)
	{
		if (GetOwnerRole() == ROLE_Authority)
		{
			if (bIsUsingSkillNow) return;
			bIsUsingSkillNow = true;
			bCanUseUltAttackSkill = false;
			OnRep_CanUseUltAttackSkill();

			SkillInputStop();
			MC_AxeUltAttack(InPlayerWeapon);
			UltAttackSkillCoolTimer();
		}
		else
		{
			SkillInputStop();
			Server_AxeUltAttack(InPlayerWeapon);
			UltAttackSkillCoolTimer();
		}
	}
}

void USkillComponent::Server_AxeUltAttack_Implementation(AValhallaWeaponBase* InPlayerWeapon)
{
	if (bIsUsingSkillNow) return;
	bIsUsingSkillNow = true;
	bCanUseUltAttackSkill = false;
	OnRep_CanUseUltAttackSkill();

	MC_AxeUltAttack(InPlayerWeapon);
}

void USkillComponent::MC_AxeUltAttack_Implementation(AValhallaWeaponBase* InPlayerWeapon)
{
	if (AValhallaWeaponAxe* Axe = Cast<AValhallaWeaponAxe>(InPlayerWeapon))
	{
		CharacterSkills[4]->ActivateSkill(GetOwner<AValhallaPlayerCharacter>(), TargetActor);
	}
}
#pragma endregion
