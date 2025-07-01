// Fill out your copyright notice in the Description page of Project Settings.


#include "Animation/ValhallaAnimInstance.h"
#include "Character/ValhallaCharacterBase.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "KismetAnimationLibrary.h"

UValhallaAnimInstance::UValhallaAnimInstance()
{

}

void UValhallaAnimInstance::NativeInitializeAnimation()
{
	OwningCharacter = Cast<AValhallaCharacterBase>(TryGetPawnOwner());

	if (OwningCharacter)
	{
		OwningMovementComponent = OwningCharacter->GetCharacterMovement();
	}
}

void UValhallaAnimInstance::NativeThreadSafeUpdateAnimation(float DeltaSeconds)
{
	if (!OwningCharacter || !OwningMovementComponent)
		return;

	Velocity = OwningCharacter->GetVelocity();

	GroundSpeed = Velocity.Size2D();

	bHasAcceleration = OwningMovementComponent->GetCurrentAcceleration().SizeSquared2D() > 0.f;

	bIsFalling = OwningMovementComponent->IsFalling();

	LocomotionDirection = UKismetAnimationLibrary::CalculateDirection(Velocity, OwningCharacter->GetActorRotation());

	PlayerCurrentState = OwningCharacter->GetPlayerCurrentState();

	IsOnBoardCannon = OwningCharacter->IsCannon;

	BallistaAngle = OwningCharacter->BallistaRotationPitch;
}
