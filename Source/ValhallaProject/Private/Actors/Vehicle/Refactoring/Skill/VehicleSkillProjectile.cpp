// Fill out your copyright notice in the Description page of Project Settings.


#include "Actors/Vehicle/Refactoring/Skill/VehicleSkillProjectile.h"

#include "GameFramework/ProjectileMovementComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"
#include "Kismet/KismetMathLibrary.h"

#include "Interface/ActorTypeInterface.h"

AVehicleSkillProjectile::AVehicleSkillProjectile()
{
	PrimaryActorTick.bCanEverTick = false;
	PrimaryActorTick.bStartWithTickEnabled = false;

	bReplicates = true;

	ProjectielMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ProjectielMesh"));
	SetRootComponent(ProjectielMesh);
	ProjectielMesh->OnComponentHit.AddDynamic(this, &AVehicleSkillProjectile::OnProjectileMeshHit);

	ProjectileMovementComp = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovementComp"));
	ProjectileMovementComp->InitialSpeed = 1000.f;
	ProjectileMovementComp->MaxSpeed = 1000.f;
	ProjectileMovementComp->Velocity = FVector(1.f, 0.f, 0.f);
	ProjectileMovementComp->ProjectileGravityScale = 1.f;

	TeamType = EPlayerTeamType::None;

	Damage = 200.f;

	Life = 1.f;

	HitEffectScale = FVector(4.f, 4.f, 4.f);

	HitLocation = FVector(0.f, 0.f, 0.f);
}

void AVehicleSkillProjectile::BeginPlay()
{
	Super::BeginPlay();
	
	SetLifeSpan(Life);
}

void AVehicleSkillProjectile::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AVehicleSkillProjectile, HitLocation);
	DOREPLIFETIME(AVehicleSkillProjectile, HitNormalRotation);
}

void AVehicleSkillProjectile::Destroyed()
{
	Super::Destroyed();


}

void AVehicleSkillProjectile::OnProjectileMeshHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	if (HasAuthority())
	{
		PlayHitSound();

		AController* OwnerPawnController = GetOwner<APawn>()->GetController();

		if (IActorTypeInterface* ActorTypeInterface = Cast<IActorTypeInterface>(OtherActor))
		{
			if (ActorTypeInterface->GetActorTypeTag() == EActorType::Construction || ActorTypeInterface->GetActorTypeTag() == EActorType::Turret || ActorTypeInterface->GetActorTypeTag() == EActorType::GatewayTurret)
			{
				float FinalDamage = Damage * 1.5f;
				UGameplayStatics::ApplyDamage(OtherActor, FinalDamage, OwnerPawnController, this, nullptr);
			}
			else if (ActorTypeInterface->GetActorTypeTag() == EActorType::Player || ActorTypeInterface->GetActorTypeTag() == EActorType::Minion)
			{
				float FinalDamage = Damage;
				UGameplayStatics::ApplyDamage(OtherActor, FinalDamage, OwnerPawnController, this, nullptr);
			}
			else if (ActorTypeInterface->GetActorTypeTag() == EActorType::Vehicle)
			{
				float FinalDamage = Damage * 1.2f;
				UGameplayStatics::ApplyDamage(OtherActor, FinalDamage, OwnerPawnController, this, nullptr);
			}
		}

		HitLocation = Hit.ImpactPoint;
		HitNormalRotation = UKismetMathLibrary::MakeRotFromZ(Hit.ImpactNormal);

		PlayHitEffect(HitLocation, HitNormalRotation);
		SetActorHiddenInGame(true);
		SetActorEnableCollision(false);

		AfterHit();
	}
}

void AVehicleSkillProjectile::PlayHitEffect(FVector InHitLocation, FRotator InHitNormalRotation)
{
	if (ProjectileHitEffect)
	{
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(this, ProjectileHitEffect, InHitLocation, InHitNormalRotation, HitEffectScale);
	}
}

void AVehicleSkillProjectile::OnRep_HitLocation()
{
	PlayHitEffect(HitLocation, HitNormalRotation);
	SetActorHiddenInGame(true);
	SetActorEnableCollision(false);
}

void AVehicleSkillProjectile::PlayHitSound_Implementation()
{
	if (HitSound != nullptr)
	{
		UGameplayStatics::SpawnSoundAtLocation(this, HitSound, GetActorLocation());
	}
}
