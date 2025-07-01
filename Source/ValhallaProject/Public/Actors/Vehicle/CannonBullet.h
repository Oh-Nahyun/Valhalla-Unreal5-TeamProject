// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Others/Enums.h"
#include "CannonBullet.generated.h"

class UNiagaraSystem;
class UProjectileMovementComponent;
class USoundBase;

UCLASS()
class VALHALLAPROJECT_API ACannonBullet : public AActor
{
	GENERATED_BODY()
	
public:	
	ACannonBullet();

protected:
	virtual void BeginPlay() override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void Destroyed() override;


#pragma region Component Section
private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Component, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UStaticMeshComponent> BulletMesh;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Component, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UNiagaraSystem> ProjectileHitEffect;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = Component, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UProjectileMovementComponent> ProjectileMovementComp;

	UFUNCTION()
	void OnBulletMeshHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);
#pragma endregion


#pragma region Enum Section
private:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Bullet, meta = (AllowPrivateAccess = "true"))
	EPlayerTeamType TeamType;

public:
	FORCEINLINE void SetTeamType(const EPlayerTeamType InPlayerTeamType) { TeamType = InPlayerTeamType; }
#pragma endregion


#pragma region Bullet Section
private:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Bullet, meta = (AllowPrivateAccess = "true"))
	float BulletDamage;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Bullet, meta = (AllowPrivateAccess = "true"))
	FVector HitEffectScale;

	void PlayHitEffect(FVector InHitLocation, FRotator InHitNormalRotation);

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Bullet, meta = (AllowPrivateAccess = "true"), ReplicatedUsing = OnRep_HitLocation)
	FVector HitLocation;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Bullet, meta = (AllowPrivateAccess = "true"), Replicated)
	FRotator HitNormalRotation;

public:
	FORCEINLINE void SetHitEffectScale(const FVector InNewScale) { HitEffectScale = InNewScale; }

	UFUNCTION()
	void OnRep_HitLocation();

protected:
	UFUNCTION(BlueprintImplementableEvent)
	void AfterHit();

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Sound, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<USoundBase> HitSound;

	UFUNCTION(NetMulticast, Unreliable)
	void PlayHitSound();
#pragma endregion
};
