// Fill out your copyright notice in the Description page of Project Settings.


#include "Actors/Vehicle/CannonBullet.h"

#include "GameFramework/ProjectileMovementComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"
#include "Kismet/KismetMathLibrary.h"

#include "Interface/ActorTypeInterface.h"

ACannonBullet::ACannonBullet()
{
	PrimaryActorTick.bCanEverTick = false;
	PrimaryActorTick.bStartWithTickEnabled = false;

	bReplicates = true;

	BulletMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BulletMesh"));
	SetRootComponent(BulletMesh);
	BulletMesh->OnComponentHit.AddDynamic(this, &ACannonBullet::OnBulletMeshHit);

	ProjectileMovementComp = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovementComp"));
	ProjectileMovementComp->InitialSpeed = 1000.f;
	ProjectileMovementComp->MaxSpeed = 1000.f;
	ProjectileMovementComp->Velocity = FVector(1.f, 0.f, 0.f);
	ProjectileMovementComp->ProjectileGravityScale = 1.f;

	TeamType = EPlayerTeamType::None;

	BulletDamage = 200.f;

	HitEffectScale = FVector(1.f, 1.f, 1.f);

	HitLocation = FVector(0.f, 0.f, 0.f);
}

void ACannonBullet::BeginPlay()
{
	Super::BeginPlay();
	
}

void ACannonBullet::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ACannonBullet, HitLocation);
	DOREPLIFETIME(ACannonBullet, HitNormalRotation);
}

void ACannonBullet::Destroyed()
{
	Super::Destroyed();

}

void ACannonBullet::OnBulletMeshHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	if (HasAuthority())
	{
		PlayHitSound();

		AController* OwnerPawnController = GetOwner<APawn>()->GetController();

		if (IActorTypeInterface* ActorTypeInterface = Cast<IActorTypeInterface>(OtherActor))
		{
			if (ActorTypeInterface->GetActorTypeTag() == EActorType::Construction || ActorTypeInterface->GetActorTypeTag() == EActorType::Turret || ActorTypeInterface->GetActorTypeTag() == EActorType::GatewayTurret)
			{
				float FinalDamage = BulletDamage * 1.5f;
				UGameplayStatics::ApplyDamage(OtherActor, FinalDamage, OwnerPawnController, this, nullptr);
			}
			else if (ActorTypeInterface->GetActorTypeTag() == EActorType::Player || ActorTypeInterface->GetActorTypeTag() == EActorType::Minion)
			{
				float FinalDamage = BulletDamage;
				UGameplayStatics::ApplyDamage(OtherActor, FinalDamage, OwnerPawnController, this, nullptr);
			}
			else if (ActorTypeInterface->GetActorTypeTag() == EActorType::Vehicle)
			{
				float FinalDamage = BulletDamage * 1.2f;
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

void ACannonBullet::PlayHitEffect(FVector InHitLocation, FRotator InHitNormalRotation)
{
	if (ProjectileHitEffect)
	{
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(this, ProjectileHitEffect, InHitLocation, InHitNormalRotation, HitEffectScale);
	}
}

void ACannonBullet::OnRep_HitLocation()
{
	PlayHitEffect(HitLocation, HitNormalRotation);
	SetActorHiddenInGame(true);
	SetActorEnableCollision(false);
}

void ACannonBullet::PlayHitSound_Implementation()
{
	if (HitSound != nullptr)
	{
		UGameplayStatics::SpawnSoundAtLocation(this, HitSound, GetActorLocation());
	}
}

