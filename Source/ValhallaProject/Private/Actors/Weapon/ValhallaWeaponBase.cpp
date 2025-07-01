// Fill out your copyright notice in the Description page of Project Settings.


#include "Actors/Weapon/ValhallaWeaponBase.h"

#include "Components/BoxComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/DamageEvents.h"
#include "Net/UnrealNetwork.h"
#include "Kismet/KismetSystemLibrary.h"
#include "NiagaraFunctionLibrary.h"

#include "Others/Structs.h"
#include "Interface/CombatInterface.h"
#include "Interface/TeamInterface.h"
#include "Interface/ActorTypeInterface.h"

#include "Others/Debug.h"

AValhallaWeaponBase::AValhallaWeaponBase()
{
	PrimaryActorTick.bCanEverTick = false;
	PrimaryActorTick.bStartWithTickEnabled = false;

	bReplicates = true;

	WeaponMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("WeaponMesh"));
	SetRootComponent(WeaponMesh);

	WeaponCollisionBox = CreateDefaultSubobject<UBoxComponent>(TEXT("WeaponCollisionBox"));
	WeaponCollisionBox->SetupAttachment(WeaponMesh);
	WeaponCollisionBox->SetBoxExtent(FVector(20.f));
	WeaponCollisionBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	WeaponCollisionBox->OnComponentBeginOverlap.AddUniqueDynamic(this, &ThisClass::OnCollisionBoxBeginOverlap);
	WeaponCollisionBox->OnComponentEndOverlap.AddUniqueDynamic(this, &ThisClass::OnCollisionBoxEndOverlap);

	WeaponLevel = 1;
	WeaponMaxLevel = 3;
	WeaponType = EPlayerWeaponType::Axe;
}

void AValhallaWeaponBase::BeginPlay()
{
	Super::BeginPlay();
	
	
}

void AValhallaWeaponBase::PostInitProperties()
{
	Super::PostInitProperties();

}

void AValhallaWeaponBase::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AValhallaWeaponBase, WeaponPhysicalAttack);
	DOREPLIFETIME(AValhallaWeaponBase, WeaponMagicalAttack);
	DOREPLIFETIME(AValhallaWeaponBase, AdditionalPhysicalDefence);
	DOREPLIFETIME(AValhallaWeaponBase, AdditionalMagicalDefence);
	DOREPLIFETIME(AValhallaWeaponBase, WeaponAttackSpeed);
	DOREPLIFETIME(AValhallaWeaponBase, WeaponLevel);
	DOREPLIFETIME(AValhallaWeaponBase, WeaponMaxLevel);
}

void AValhallaWeaponBase::OnCollisionBoxBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	AActor* OwningActor = GetOwner();

	if (HasAuthority())
	{
		check(OwningActor);

		ICombatInterface* CombatInterface = Cast<ICombatInterface>(OwningActor);

		if (OtherActor == OwningActor)
		{
			return;
		}

		if (HitActors.Contains(OtherActor))
		{
			return;
		}

		if (!IsSameTeam(OwningActor, OtherActor))
		{
			return;
		}

		if (IActorTypeInterface* ActorTypeInterface = Cast<IActorTypeInterface>(OtherActor))
		{
			if (ActorTypeInterface->GetActorTypeTag() == EActorType::Player || ActorTypeInterface->GetActorTypeTag() == EActorType::Minion)
			{
				ServerHitEffect(ActorTypeInterface->GetActorTypeTag());
			}
			else if (ActorTypeInterface->GetActorTypeTag() == EActorType::Construction || ActorTypeInterface->GetActorTypeTag() == EActorType::Turret || ActorTypeInterface->GetActorTypeTag() == EActorType::GatewayTurret)
			{
				ServerHitEffect(ActorTypeInterface->GetActorTypeTag());
			}
			else if (ActorTypeInterface->GetActorTypeTag() == EActorType::Vehicle)
			{
				ServerHitEffect(ActorTypeInterface->GetActorTypeTag());
			}
		}

		if (OtherActor && CombatInterface)
		{
			HitActors.AddUnique(OtherActor);

			float Damage = CombatInterface->GetCharacterAttackPower() + WeaponPhysicalAttack;
			AController* DamageInstigator = GetOwner<APawn>()->GetController();

			UGameplayStatics::ApplyDamage(OtherActor, Damage, DamageInstigator, this, nullptr);
		}
	}
}

void AValhallaWeaponBase::OnCollisionBoxEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{

}

bool AValhallaWeaponBase::IsSameTeam(AActor* InOwnerActor, AActor* InOtherActor)
{
	ITeamInterface* OwnerTeamInterface = Cast<ITeamInterface>(InOwnerActor);
	ITeamInterface* OtherTeamInterface = Cast<ITeamInterface>(InOtherActor);

	check(OwnerTeamInterface && OtherTeamInterface);

	return OwnerTeamInterface->GetActorTeamType() != OtherTeamInterface->GetActorTeamType();
}

void AValhallaWeaponBase::Initialize()
{	
	ensure(WeaponStatusData);

	SetWeaponStatus(WeaponLevel);
}

void AValhallaWeaponBase::SetWeaponStatus(const uint8 InWeaponLevel)
{
	FWeaponStatusDataTable* FoundedRow = WeaponStatusData->FindRow<FWeaponStatusDataTable>(FName(FString::FromInt(InWeaponLevel)), TEXT("Cannot find DataTable"));
	if (FoundedRow)
	{
		WeaponPhysicalAttack = FoundedRow->PhysicalAttack;
		WeaponMagicalAttack = FoundedRow->MagicalAttack;
		AdditionalPhysicalDefence = FoundedRow->PhysicalDefense;
		AdditionalMagicalDefence = FoundedRow->MagicalDefense;
		WeaponAttackSpeed = FoundedRow->AttackSpeed;
	}
}

void AValhallaWeaponBase::WeaponUpgrade()
{
	if (WeaponLevel <= WeaponMaxLevel - 1)
	{
		WeaponLevel += 1;

		SetWeaponStatus(WeaponLevel);
	}
}

void AValhallaWeaponBase::ClearHitActors()
{
	HitActors.Empty();
}

