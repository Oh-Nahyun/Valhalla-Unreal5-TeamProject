// Fill out your copyright notice in the Description page of Project Settings.


#include "Actors/Spawner/MinionSpawner.h"

AMinionSpawner::AMinionSpawner()
{
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = true;

	SpawnerMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("SpawnerMesh"));
	SetRootComponent(SpawnerMesh);

	SpawnLocation = CreateDefaultSubobject<USceneComponent>(TEXT("SpawnLocation"));
	SpawnLocation->SetupAttachment(GetRootComponent());
}

void AMinionSpawner::BeginPlay()
{
	Super::BeginPlay();
	
}

void AMinionSpawner::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

