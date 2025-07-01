// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/ValhallaCharacterBase.h"

#include "Components/CapsuleComponent.h"
#include "Net/UnrealNetwork.h"

#include "Actors/Weapon/ValhallaWeaponBase.h"

#include "Others/Debug.h"

AValhallaCharacterBase::AValhallaCharacterBase()
{
	PrimaryActorTick.bCanEverTick = true;

	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	TeamType = EPlayerTeamType::None;

	PlayerCurrentState = EPlayerCurrentState::Alive;

	IsCannon = false;
	BallistaRotationPitch = 0.f;
}

void AValhallaCharacterBase::BeginPlay()
{
	Super::BeginPlay();

	if (TeamType == EPlayerTeamType::Red)
	{
		GetCapsuleComponent()->SetCollisionObjectType(ECollisionChannel::ECC_GameTraceChannel3);
	}
	else if (TeamType == EPlayerTeamType::Blue)
	{
		GetCapsuleComponent()->SetCollisionObjectType(ECollisionChannel::ECC_GameTraceChannel4);
	}
}

void AValhallaCharacterBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AValhallaCharacterBase::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void AValhallaCharacterBase::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

}

void AValhallaCharacterBase::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();

}

float AValhallaCharacterBase::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

	return DamageAmount;
}

void AValhallaCharacterBase::OnRep_PlayerCurrentState()
{
	if (PlayerCurrentState == EPlayerCurrentState::Dead)
	{
		if (GetMesh()->IsPlaying())
		{
			GetMesh()->GetAnimInstance()->StopAllMontages(.2f);
		}

		GetMesh()->GetAnimInstance()->Montage_Play(DeadMontage);
	}
}

void AValhallaCharacterBase::OnRep_TeamType()
{
	if (TeamType == EPlayerTeamType::Red)
	{
		GetCapsuleComponent()->SetCollisionObjectType(ECollisionChannel::ECC_GameTraceChannel3);
	}
	else if (TeamType == EPlayerTeamType::Blue)
	{
		GetCapsuleComponent()->SetCollisionObjectType(ECollisionChannel::ECC_GameTraceChannel4);
	}
}

void AValhallaCharacterBase::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AValhallaCharacterBase, CurrentHealth);

	DOREPLIFETIME(AValhallaCharacterBase, TeamType);

	DOREPLIFETIME(AValhallaCharacterBase, PlayerCurrentState);
	DOREPLIFETIME(AValhallaCharacterBase, GoldGivenAmount);

	DOREPLIFETIME(AValhallaCharacterBase, IsCannon);
	DOREPLIFETIME(AValhallaCharacterBase, BallistaRotationPitch);
}

void AValhallaCharacterBase::OnRep_CurrentHealth()
{
}

void AValhallaCharacterBase::InitializeHealthAndMana()
{
}

void AValhallaCharacterBase::ResetHealthAndMana()
{
}

void AValhallaCharacterBase::Die()
{
}

bool AValhallaCharacterBase::IsDead(const float InCurrentHealth)
{
	if (InCurrentHealth <= 0)
	{
		return true;
	}

	return false;
}

void AValhallaCharacterBase::SetCollisionOnOff_Implementation(const bool IsCollisionOn)
{
	if (!IsCollisionOn)
	{
		GetCapsuleComponent()->SetCollisionResponseToChannel(ECollisionChannel::ECC_GameTraceChannel3, ECollisionResponse::ECR_Overlap);
		GetCapsuleComponent()->SetCollisionResponseToChannel(ECollisionChannel::ECC_GameTraceChannel4, ECollisionResponse::ECR_Overlap);
		GetCapsuleComponent()->SetCollisionResponseToChannel(ECollisionChannel::ECC_GameTraceChannel2, ECollisionResponse::ECR_Ignore);
	}
	else
	{
		GetCapsuleComponent()->SetCollisionResponseToChannel(ECollisionChannel::ECC_GameTraceChannel3, ECollisionResponse::ECR_Block);
		GetCapsuleComponent()->SetCollisionResponseToChannel(ECollisionChannel::ECC_GameTraceChannel4, ECollisionResponse::ECR_Block);
		GetCapsuleComponent()->SetCollisionResponseToChannel(ECollisionChannel::ECC_GameTraceChannel2, ECollisionResponse::ECR_Overlap);
	}
}

void AValhallaCharacterBase::ToggleWeaponCollision(bool IsCollisionEnable)
{
}

EPlayerTeamType& AValhallaCharacterBase::GetActorTeamType()
{
	return TeamType;
}

EActorType& AValhallaCharacterBase::GetActorTypeTag()
{
	return ActorType;
}

