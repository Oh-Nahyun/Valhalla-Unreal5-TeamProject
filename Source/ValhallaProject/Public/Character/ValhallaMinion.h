// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/ValhallaCharacterBase.h"
#include "ValhallaMinion.generated.h"

class AController;
class USphereComponent;
class UChildActorComponent;
class AMeleeMinionWeapon;
class UBoxComponent;
class ARangeMinionProjectile;
class USoundBase;

/**
 * 
 */
UCLASS()
class VALHALLAPROJECT_API AValhallaMinion : public AValhallaCharacterBase
{
	GENERATED_BODY()
	
public:
	AValhallaMinion();

protected:
	virtual void BeginPlay() override;
	virtual void PossessedBy(AController* NewController) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;


#pragma region Components Section
private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Components, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<USphereComponent> SphereCollision;

	UFUNCTION()
	virtual void OnCollisionBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	virtual void OnCollisionEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
#pragma endregion


#pragma region Target Section
protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = PawnAI, meta = (AllowPrivateAccess = "true"), Replicated)
	TArray<AActor*> TargetActors;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = PawnAI, meta = (AllowPrivateAccess = "true"), Replicated)
	TObjectPtr<AActor> TargetActor;
#pragma endregion


#pragma region Status Section
public:
	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = Status, meta = (AllowPrivateAccess = "true"), Replicated)
	float MaxHealth;

	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = Status, meta = (AllowPrivateAccess = "true"), Replicated)
	float ExpGivenAmount;

	UPROPERTY(EditAnyWhere, BlueprintReadOnly, Category = Status, meta = (AllowPrivateAccess = "true"))
	EMinionType MinionType;

protected:
	virtual void InitializeHealthAndMana() override;

private:
	void DamagedMinionHealth(const float InDamage);
#pragma endregion


#pragma region Death Section
protected:
	virtual void Die() override;

	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = Status, meta = (AllowPrivateAccess = "true"), ReplicatedUsing = OnRep_IsMinionDead)
	uint8 IsMinionDead : 1;

private:
	UFUNCTION()
	void OnRep_IsMinionDead();

	void SetCollisionOff();

public:
	UFUNCTION(BlueprintCallable)
	void DestroyMinion();
#pragma endregion


#pragma region Delegate Section
private:
	UFUNCTION(NetMulticast, Unreliable)
	void OnCharacterHealthChanged(float InCurrentHealth, float InMaxHealth, AController* InInstigatorController);
#pragma endregion


#pragma region Interface Section
protected:
	virtual void ToggleWeaponCollision(bool IsCollisionEnable) override;
#pragma endregion


#pragma region Weapon And Damage Section
private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Components, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<AMeleeMinionWeapon> MinionWeapon;

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Component)
	TObjectPtr<UStaticMeshComponent> WeaponMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Component)
	TObjectPtr<UBoxComponent> WeaponCollisionBox;

private:
	UFUNCTION()
	virtual void OnWeaponCollisionBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

protected:
	UFUNCTION(BlueprintImplementableEvent)
	void ServerHitEffect(AActor* InOtherActor);

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Damage)
	float MeleeMinionDamage;

public:
	UFUNCTION(BlueprintCallable)
	void RangeAttack();

private:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = ProjectileClass, meta = (AllowPrivateAccess = "true"))
	TSubclassOf<ARangeMinionProjectile> RangeMinionProjectileClass;
#pragma endregion


#pragma region Sound Section
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Sound, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<USoundBase> AttackSound;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Sound, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<USoundBase> HitSound;

	UFUNCTION(NetMulticast, Unreliable)
	void PlayHitSound();
#pragma endregion
};
