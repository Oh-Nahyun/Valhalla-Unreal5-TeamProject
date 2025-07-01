// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Others/Enums.h"
#include "ValhallaTurretBullet.generated.h"

class USphereComponent;
class UNiagaraComponent;
class UProjectileMovementComponent;
class UNiagaraSystem;
class USoundBase;

UCLASS()
class VALHALLAPROJECT_API AValhallaTurretBullet : public AActor
{
	GENERATED_BODY()
	
public:	
	AValhallaTurretBullet();

protected:
	virtual void BeginPlay() override;

protected:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void Destroyed() override;


#pragma region Component Section
private:
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Projectile", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<USphereComponent> ProjectileCollisionSphere;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Projectile", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UNiagaraComponent> ProjectileNiagaraComponent;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Projectile", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UNiagaraSystem> ProjectileHitEffect;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Projectile", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UProjectileMovementComponent> ProjectileMovementComp;

private:
	UFUNCTION()
	void OnCollisionSphereHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);
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
	float DamageRadius;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Bullet, meta = (AllowPrivateAccess = "true"))
	FVector HitEffectScale;

	void PlayHitEffect(FVector InHitLocation);

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Bullet, meta = (AllowPrivateAccess = "true"), ReplicatedUsing = OnRep_HitLocation)
	FVector HitLocation;

	UFUNCTION()
	void OnRep_HitLocation();

public:
	FORCEINLINE void SetHitEffectScale(const FVector InNewScale) { HitEffectScale = InNewScale; }

protected:
	UFUNCTION(BlueprintImplementableEvent)
	void AfterHit();

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Sound, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<USoundBase> HitSound;

	UFUNCTION(NetMulticast, Unreliable)
	void PlayHitSound();
#pragma endregion
};
