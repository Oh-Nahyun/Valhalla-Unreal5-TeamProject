// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Others/Enums.h"
#include "Interface/TeamInterface.h"
#include "Interface/ActorTypeInterface.h"
#include "ValhallaConstruction.generated.h"

DECLARE_DELEGATE_ThreeParams(FOnConsturctionHealthChangedSignature, float, float, AController*);

class UWidgetComponent;

UCLASS()
class VALHALLAPROJECT_API AValhallaConstruction : public AActor, public ITeamInterface, public IActorTypeInterface
{
	GENERATED_BODY()
	
public:	
	AValhallaConstruction();

protected:
	virtual void BeginPlay() override;

public:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;
	virtual void Destroyed() override;


#pragma region Delegate Section
public:
	FOnConsturctionHealthChangedSignature OnConstructionHealthChangedDelegate;
#pragma endregion


#pragma region Component Section
protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Component, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UStaticMeshComponent> ConstructionMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Component, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UWidgetComponent> ConstructionWidgetComponent;
#pragma endregion


#pragma region Enum Section
private:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Enums, meta = (AllowPrivateAccess = "true"))
	EPlayerTeamType TeamType;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Enums, meta = (AllowPrivateAccess = "true"))
	EActorType ActorType;

public:
	FORCEINLINE EPlayerTeamType& GetTeamType() { return TeamType; }
	FORCEINLINE EActorType& GetActorType() { return ActorType; }
#pragma endregion


#pragma region Status Section
protected:
	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = Status, meta = (AllowPrivateAccess = "true"), Replicated)
	float CurrentHealth;

	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = Status, meta = (AllowPrivateAccess = "true"))
	float MaxHealth;

	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = Status, meta = (AllowPrivateAccess = "true"))
	float ConstructionDefense;

	UPROPERTY(EditAnyWhere, BlueprintReadOnly, Category = Status, meta = (AllowPrivateAccess = "true"))
	int32 GoldGivenAmount;

private:
	void InitializeStatus();
#pragma endregion


#pragma region Destroy Section
private:
	void DestroyConstruction();

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Sound, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<USoundBase> DestroySound;
#pragma endregion


#pragma region Interface Section
public:
	virtual EPlayerTeamType& GetActorTeamType() override;

	UFUNCTION(BlueprintCallable)
	virtual EActorType& GetActorTypeTag() override;
#pragma endregion


public:
	UFUNCTION(NetMulticast, Reliable)
	void OnConstructionHealthChanged(float InCurrentHealth, float InMaxHealth, AController* InInstigatorController);
};
