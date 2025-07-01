// Fill out your copyright notice in the Description page of Project Settings.


#include "Actors/Construction/ValhallaConstruction.h"

#include "Net/UnrealNetwork.h"
#include "Components/WidgetComponent.h"
#include "Kismet/GameplayStatics.h"

#include "Widget/ValhallaHealthWidget.h"
#include "Interface/GoldInterface.h"
#include "Player/ValhallaPlayerController.h"

#include "Others/Debug.h"

AValhallaConstruction::AValhallaConstruction()
{
	PrimaryActorTick.bCanEverTick = false;
	PrimaryActorTick.bStartWithTickEnabled = false;

	bReplicates = true;

	ConstructionMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ConstructionMesh"));
	SetRootComponent(ConstructionMesh);

	ConstructionWidgetComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("ConstructionWidgetComponent"));
	ConstructionWidgetComponent->SetupAttachment(RootComponent);

	MaxHealth = 500.f;
	ConstructionDefense = 10.f;

	TeamType = EPlayerTeamType::None;

	ActorType = EActorType::Construction;

	GoldGivenAmount = 150;
}

void AValhallaConstruction::BeginPlay()
{
	Super::BeginPlay();
	
	if (HasAuthority())
	{
		InitializeStatus();
	}

	UValhallaHealthWidget* HealthWidget = Cast<UValhallaHealthWidget>(ConstructionWidgetComponent->GetWidget());
	if (HealthWidget)
	{
		OnConstructionHealthChangedDelegate.BindUObject(HealthWidget, &UValhallaHealthWidget::SetHealthBar);
	}
}

void AValhallaConstruction::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AValhallaConstruction, CurrentHealth);
}

float AValhallaConstruction::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	if (EventInstigator == nullptr)
	{
		return 0.f;
	}

	APawn* InstigatorPawn = EventInstigator->GetPawn<APawn>();

	if (ITeamInterface* TeamInterface = Cast<ITeamInterface>(InstigatorPawn))
	{
		if (TeamInterface->GetActorTeamType() == TeamType)
		{
			return 0.f;
		}
	}

	Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

	float Damage = FMath::Clamp(DamageAmount - ConstructionDefense, 0, DamageAmount);

	if (HasAuthority())
	{
		if (Damage > 0)
		{
			float NewCurrentHealth = FMath::Clamp(CurrentHealth - Damage, 0, CurrentHealth);
			CurrentHealth = NewCurrentHealth;

			if (IActorTypeInterface* ActorTypeInterface = Cast<IActorTypeInterface>(InstigatorPawn))
			{
				if (ActorTypeInterface->GetActorTypeTag() == EActorType::Player || ActorTypeInterface->GetActorTypeTag() == EActorType::Vehicle)
				{
					OnConstructionHealthChanged(CurrentHealth, MaxHealth, EventInstigator);
				}
			}
		}

		if (CurrentHealth <= 0)
		{
			if (AValhallaPlayerController* InstigatorController = Cast<AValhallaPlayerController>(EventInstigator))
			{
				if (IGoldInterface* PlayerCharacterGoldInterface = Cast<IGoldInterface>(InstigatorController->ControlledPawn))
				{
					PlayerCharacterGoldInterface->GainExpAndGoldInterface(0, GoldGivenAmount);
				}
			}

			DestroyConstruction();
		}
	}

	return Damage;
}

void AValhallaConstruction::Destroyed()
{
	Super::Destroyed();

	if (DestroySound != nullptr)
	{
		UGameplayStatics::PlaySoundAtLocation(this, DestroySound, GetActorLocation());
	}
}

void AValhallaConstruction::InitializeStatus()
{
	CurrentHealth = MaxHealth;
}

void AValhallaConstruction::DestroyConstruction()
{
	Destroy();
}

EPlayerTeamType& AValhallaConstruction::GetActorTeamType()
{
	return TeamType;
}

EActorType& AValhallaConstruction::GetActorTypeTag()
{
	return ActorType;
}

void AValhallaConstruction::OnConstructionHealthChanged_Implementation(float InCurrentHealth, float InMaxHealth, AController* InInstigatorController)
{
	OnConstructionHealthChangedDelegate.ExecuteIfBound(InCurrentHealth, InMaxHealth, InInstigatorController);
}

