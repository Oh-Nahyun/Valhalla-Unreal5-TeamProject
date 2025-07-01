// Fill out your copyright notice in the Description page of Project Settings.


#include "AnimNotify/ANS_WeaponCollisionEnable.h"

#include "Interface/WeaponInterface.h"

void UANS_WeaponCollisionEnable::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyBegin(MeshComp, Animation, TotalDuration, EventReference);

	IWeaponInterface* WeaponInterface = Cast<IWeaponInterface>(MeshComp->GetOwner());
	if (WeaponInterface)
	{
		WeaponInterface->ToggleWeaponCollision(true);
	}
}

void UANS_WeaponCollisionEnable::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyEnd(MeshComp, Animation, EventReference);

	IWeaponInterface* WeaponInterface = Cast<IWeaponInterface>(MeshComp->GetOwner());
	if (WeaponInterface)
	{
		WeaponInterface->ToggleWeaponCollision(false);
	}
}

