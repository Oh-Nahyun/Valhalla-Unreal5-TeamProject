// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Others/Enums.h"
#include "RangeMinionProjectile.generated.h"

class USphereComponent;
class UNiagaraComponent;
class UProjectileMovementComponent;

UCLASS()
class VALHALLAPROJECT_API ARangeMinionProjectile : public AActor
{
	GENERATED_BODY()
	
public:	
	ARangeMinionProjectile();

protected:
	virtual void BeginPlay() override;


#pragma region Component Section
private:
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Projectile", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<USphereComponent> ProjectileCollisionSphere;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Projectile", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UNiagaraComponent> ProjectileNiagaraComponent;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Projectile", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UProjectileMovementComponent> ProjectileMovementComp;

private:
	UFUNCTION()
	virtual void OnCollisionBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

public:
	void SetBulletCollision(EPlayerTeamType& InTeamType);
#pragma endregion


#pragma region Enum Section
private:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Types, meta = (AllowPrivateAccess = "true"))
	EPlayerTeamType TeamType;

public:
	FORCEINLINE void SetTeamType(const EPlayerTeamType InPlayerTeamType) { TeamType = InPlayerTeamType; }
#pragma endregion


#pragma region Bullet Section
private:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Damage, meta = (AllowPrivateAccess = "true"))
	float BulletDamage;
#pragma endregion
};
