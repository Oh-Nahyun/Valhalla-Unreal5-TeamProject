// Fill out your copyright notice in the Description page of Project Settings.


#include "Actors/Turret/ValhallaTurretBullet.h"

#include "Components/SphereComponent.h"
#include "NiagaraComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Components/SceneComponent.h"
#include "Net/UnrealNetwork.h"
#include "Kismet/GameplayStatics.h"
#include "NiagaraFunctionLibrary.h"

#include "Interface/ActorTypeInterface.h"

#include "Others/Debug.h"

AValhallaTurretBullet::AValhallaTurretBullet()
{
	PrimaryActorTick.bCanEverTick = false;
	PrimaryActorTick.bStartWithTickEnabled = false;

	bReplicates = true;

	ProjectileCollisionSphere = CreateDefaultSubobject<USphereComponent>(TEXT("ProjectileCollisionSphere"));
	SetRootComponent(ProjectileCollisionSphere);

	ProjectileCollisionSphere->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	ProjectileCollisionSphere->SetCollisionResponseToChannel(ECC_Pawn, ECR_Ignore);
	ProjectileCollisionSphere->SetCollisionResponseToChannel(ECC_GameTraceChannel3, ECR_Ignore);
	ProjectileCollisionSphere->SetCollisionResponseToChannel(ECC_GameTraceChannel4, ECR_Ignore);
	ProjectileCollisionSphere->OnComponentHit.AddDynamic(this, &AValhallaTurretBullet::OnCollisionSphereHit);

	ProjectileNiagaraComponent = CreateDefaultSubobject<UNiagaraComponent>(TEXT("ProjectileNiagaraComponent"));
	ProjectileNiagaraComponent->SetupAttachment(GetRootComponent());

	ProjectileMovementComp = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovementComp"));
	ProjectileMovementComp->InitialSpeed = 700.f;
	ProjectileMovementComp->MaxSpeed = 900.f;
	ProjectileMovementComp->Velocity = FVector(1.f, 0.f, 0.f);
	ProjectileMovementComp->ProjectileGravityScale = 0.f;

	TeamType = EPlayerTeamType::None;

	BulletDamage = 100.f;

	DamageRadius = 100.f;

	HitEffectScale = FVector(1.f, 1.f, 1.f);

	HitLocation = FVector(0.f, 0.f, 0.f);
}

void AValhallaTurretBullet::BeginPlay()
{
	Super::BeginPlay();


}

void AValhallaTurretBullet::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AValhallaTurretBullet, HitLocation);
}

void AValhallaTurretBullet::Destroyed()
{
	Super::Destroyed();

}

void AValhallaTurretBullet::OnCollisionSphereHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	if (HasAuthority())
	{
		PlayHitSound();

		TArray<AActor*> IgnoreActors;
		IgnoreActors.AddUnique(GetInstigator());

		AController* OwnerPawnController = GetOwner<APawn>()->GetController();

		UGameplayStatics::ApplyRadialDamage(this, BulletDamage, Hit.ImpactPoint, DamageRadius, nullptr, IgnoreActors, this, OwnerPawnController, true);

		HitLocation = Hit.ImpactPoint;

		PlayHitEffect(HitLocation);
		SetActorHiddenInGame(true);
		SetActorEnableCollision(false);

		AfterHit();
	}
}

void AValhallaTurretBullet::PlayHitEffect(FVector InHitLocation)
{
	if (ProjectileHitEffect)
	{
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(this, ProjectileHitEffect, InHitLocation, GetActorRotation(), HitEffectScale);
	}
}

void AValhallaTurretBullet::OnRep_HitLocation()
{
	PlayHitEffect(HitLocation);
	SetActorHiddenInGame(true);
	SetActorEnableCollision(false);
}

void AValhallaTurretBullet::PlayHitSound_Implementation()
{
	if (HitSound != nullptr)
	{
		UGameplayStatics::SpawnSoundAtLocation(this, HitSound, GetActorLocation());
	}
}

