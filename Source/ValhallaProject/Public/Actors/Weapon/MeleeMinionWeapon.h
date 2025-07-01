// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MeleeMinionWeapon.generated.h"

class UBoxComponent;

UCLASS()
class VALHALLAPROJECT_API AMeleeMinionWeapon : public AActor
{
	GENERATED_BODY()
	
public:	
	AMeleeMinionWeapon();

protected:
	virtual void BeginPlay() override;


#pragma region Component Section
protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Component)
	TObjectPtr<UStaticMeshComponent> WeaponMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Component)
	TObjectPtr<UBoxComponent> WeaponCollisionBox;

public:
	FORCEINLINE UBoxComponent* GetWeaponCollision() const { return WeaponCollisionBox; }

private:
	UFUNCTION()
	virtual void OnCollisionBoxBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
#pragma endregion

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Damage)
	float MeleeMinionDamage;

public:
	void SetMinionDamage(const float InDamage) { MeleeMinionDamage = InDamage; }
};
