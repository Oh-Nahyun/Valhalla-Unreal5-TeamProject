// Fill out your copyright notice in the Description page of Project Settings.


#include "Component/ValhallaCameraComponent.h"
#include "Kismet/KismetSystemLibrary.h"

UValhallaCameraComponent::UValhallaCameraComponent()
{
	CameraTraceDistance = 200.f;
}

AActor* UValhallaCameraComponent::GetTraceHitResult()
{
	const ETraceTypeQuery TraceChannel = UEngineTypes::ConvertToTraceType(ECollisionChannel::ECC_GameTraceChannel1);
	FHitResult HitResult;

	UKismetSystemLibrary::LineTraceSingle(
		this,
		GetOwner()->GetActorLocation(),
		GetOwner()->GetActorLocation() + (GetOwner()->GetActorForwardVector() * CameraTraceDistance),
		TraceChannel,
		false,
		TArray<AActor*>(),
		EDrawDebugTrace::Type::None,
		HitResult,
		true
	);

	if (HitResult.bBlockingHit)
	{
		return HitResult.GetActor();
	}
	else
	{
		return nullptr;
	}
}
