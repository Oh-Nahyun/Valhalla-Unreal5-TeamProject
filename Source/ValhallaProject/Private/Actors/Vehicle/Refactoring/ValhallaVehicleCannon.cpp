// Fill out your copyright notice in the Description page of Project Settings.


#include "Actors/Vehicle/Refactoring/ValhallaVehicleCannon.h"

#include "Actors/Vehicle/CannonBullet.h"

AValhallaVehicleCannon::AValhallaVehicleCannon()
{
	BulletShotLocation = CreateDefaultSubobject<USceneComponent>(TEXT("BulletShotLocation"));
	BulletShotLocation->SetupAttachment(AttackPartMesh);
}

void AValhallaVehicleCannon::CannonAttack()
{
	FVector SpawnLocation = BulletShotLocation->GetComponentLocation();
	FRotator SpawnRotator = BulletShotLocation->GetComponentRotation();
	FTransform SpawnTransform(SpawnRotator, SpawnLocation, FVector(1.f, 1.f, 1.f));

	ACannonBullet* CannonBullet = GetWorld()->SpawnActorDeferred<ACannonBullet>(CannonBulletClass, SpawnTransform, this, this, ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn);
	if (CannonBullet)
	{
		CannonBullet->SetTeamType(TeamType);

		CannonBullet->FinishSpawning(SpawnTransform);
	}
}
