// Fill out your copyright notice in the Description page of Project Settings.


#include "Actors/Weapon/RangeMinionProjectile.h"

#include "Components/SphereComponent.h"
#include "NiagaraComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Kismet/GameplayStatics.h"

ARangeMinionProjectile::ARangeMinionProjectile()
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
	ProjectileCollisionSphere->OnComponentBeginOverlap.AddDynamic(this, &ARangeMinionProjectile::OnCollisionBeginOverlap);

	ProjectileNiagaraComponent = CreateDefaultSubobject<UNiagaraComponent>(TEXT("ProjectileNiagaraComponent"));
	ProjectileNiagaraComponent->SetupAttachment(RootComponent);

	ProjectileMovementComp = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovementComp"));
	ProjectileMovementComp->InitialSpeed = 700.f;
	ProjectileMovementComp->MaxSpeed = 900.f;
	ProjectileMovementComp->Velocity = FVector(1.f, 0.f, 0.f);
	ProjectileMovementComp->ProjectileGravityScale = 0.f;

	TeamType = EPlayerTeamType::None;

	BulletDamage = 100.f;
}

void ARangeMinionProjectile::BeginPlay()
{
	Super::BeginPlay();
	
	SetLifeSpan(6.f);
}

void ARangeMinionProjectile::OnCollisionBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (HasAuthority())
	{
		if (GetOwner())
		{
			AController* OwnerPawnController = GetOwner<APawn>()->GetController();

			UGameplayStatics::ApplyDamage(OtherActor, BulletDamage, OwnerPawnController, this, nullptr);
		}

		Destroy();
	}
}

void ARangeMinionProjectile::SetBulletCollision(EPlayerTeamType& InTeamType)
{
	if (InTeamType == EPlayerTeamType::Red)
	{
		ProjectileCollisionSphere->SetCollisionResponseToChannel(ECC_GameTraceChannel4, ECR_Overlap);
	}
	else if (InTeamType == EPlayerTeamType::Blue)
	{
		ProjectileCollisionSphere->SetCollisionResponseToChannel(ECC_GameTraceChannel3, ECR_Overlap);
	}
}

