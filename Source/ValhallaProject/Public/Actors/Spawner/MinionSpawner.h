// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Others/Enums.h"
#include "MinionSpawner.generated.h"

class UStaticMeshComponent;
class USceneComponent;

UCLASS()
class VALHALLAPROJECT_API AMinionSpawner : public AActor
{
	GENERATED_BODY()
	
public:	
	AMinionSpawner();

protected:
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;


#pragma region Component Section
protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Component, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UStaticMeshComponent> SpawnerMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Component, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<USceneComponent> SpawnLocation;
#pragma endregion


#pragma region Enum Section
private:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Enums, meta = (AllowPrivateAccess = "true"))
	EPlayerTeamType TeamType;

public:
	FORCEINLINE EPlayerTeamType& GetTeamType() { return TeamType; }
#pragma endregion


#pragma region Spawn Section
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void BindSpawnMinion();
#pragma endregion
};
