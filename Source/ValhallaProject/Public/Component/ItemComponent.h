// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ItemComponent.generated.h"

class AValhallaPlayerCharacter;

UCLASS(Blueprintable, BlueprintType)
class VALHALLAPROJECT_API UItemComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UItemComponent();

protected:
	virtual void BeginPlay() override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

public:
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	
#pragma region Character Info Section
	AValhallaPlayerCharacter* PlayerCharacter;
#pragma endregion

#pragma region Character Item Data Section
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Items)
	TArray<FName> CharacterInventoryItemNames;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Items)
	TArray<class UValhallaItemEffectBase*> HornOfRagnarokEffects;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Items)
	UDataTable* CharacterItemDataTable = nullptr;

	AActor* TargetActor = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Items, Replicated)
	bool bCanPressKeyboard1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Items, Replicated)
	bool bCanPressKeyboard2;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Items, Replicated)
	bool bCanPressKeyboard3;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Items, Replicated)
	bool bCanPressKeyboard4;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Items, Replicated)
	bool bCanPressKeyboard5;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Items, Replicated)
	bool bIsPressKeyboard1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Items, Replicated)
	bool bIsPressKeyboard2;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Items, Replicated)
	bool bIsPressKeyboard3;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Items, Replicated)
	bool bIsPressKeyboard4;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Items, Replicated)
	bool bIsPressKeyboard5;

	FTimerHandle HornOfRagnarokDurationTimerHandle;
	FTimerHandle HornOfRagnarokCoolTimerHandle;
#pragma endregion

#pragma region Select Inventory Item Section
public:
	void SelectInventoryItem(FName InItemName, uint8 InInventoryNum);
#pragma endregion

	void HornOfRagnarokDurationAndCoolTimer();

	UFUNCTION(Server, Reliable)
	void Server_SetHornOfRagnarokDurationTime();

	UFUNCTION(Server, Reliable)
	void Server_SetHornOfRagnarokCoolTime();

#pragma region Item Use Section
public:
	void HornOfRagnarok(uint8 InInventoryNum);
	void BerserkerBeer(uint8 InInventoryNum);
	void GraceOfValkyrie(uint8 InInventoryNum);

protected:
	UFUNCTION(Server, Reliable)
	void Server_HornOfRagnarok(AActor* InTargetActor, uint8 InInventoryNum);

	UFUNCTION(Server, Reliable)
	void Server_BerserkerBeer(AActor* InTargetActor, uint8 InInventoryNum);

	UFUNCTION(Server, Reliable)
	void Server_GraceOfValkyrie(AActor* InTargetActor, uint8 InInventoryNum);
#pragma endregion

};
