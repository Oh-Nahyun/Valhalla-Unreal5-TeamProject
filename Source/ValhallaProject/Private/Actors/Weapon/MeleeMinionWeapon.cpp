// Fill out your copyright notice in the Description page of Project Settings.


#include "Actors/Weapon/MeleeMinionWeapon.h"

#include "Components/BoxComponent.h"
#include "Kismet/GameplayStatics.h"

#include "Others/Debug.h"

AMeleeMinionWeapon::AMeleeMinionWeapon()
{
	PrimaryActorTick.bCanEverTick = false;
	PrimaryActorTick.bStartWithTickEnabled = false;

	WeaponMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("WeaponMesh"));
	SetRootComponent(WeaponMesh);

	WeaponCollisionBox = CreateDefaultSubobject<UBoxComponent>(TEXT("WeaponCollisionBox"));
	WeaponCollisionBox->SetupAttachment(WeaponMesh);
	WeaponCollisionBox->SetBoxExtent(FVector(20.f));
	WeaponCollisionBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	WeaponCollisionBox->OnComponentBeginOverlap.AddUniqueDynamic(this, &ThisClass::OnCollisionBoxBeginOverlap);

	MeleeMinionDamage = 0.f;
}

void AMeleeMinionWeapon::BeginPlay()
{
	Super::BeginPlay();

	if (HasAuthority())
	{
		if (AController* MinionController = Cast<APawn>(GetAttachParentActor())->GetController())
		{
			UE_LOG(LogTemp, Warning, TEXT("%s"), *MinionController->GetName());
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("Null"));
		}
	}
}

void AMeleeMinionWeapon::OnCollisionBoxBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (HasAuthority())
	{
		if (AActor* MinionController = GetAttachParentActor())
		{
			UE_LOG(LogTemp, Warning, TEXT("%s"), *MinionController->GetName());
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("Null"));
		}

		/*
		AController* DamageInstigator = GetOwner<APawn>()->GetController();

		if (DamageInstigator)
		{
			Debug::Print(TEXT("DamageInstigator is not null"), 5.f);
		}
		else
		{
			Debug::Print(TEXT("DamageInstigator is null"), 5.f);
		}

		UGameplayStatics::ApplyDamage(OtherActor, MeleeMinionDamage, DamageInstigator, this, nullptr);
		*/
	}
}

