// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interface/ActorTypeInterface.h"
#include "Others/Enums.h"
#include "ValhallaSanctum.generated.h"

class UUserWidget;
class AValhallaPlayerController;
class UValhallaSanctumHUD;

UCLASS()
class VALHALLAPROJECT_API AValhallaSanctum : public AActor, public IActorTypeInterface
{
	GENERATED_BODY()
	
public:
	AValhallaSanctum();

private:
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UStaticMeshComponent> SanctumMesh;

#pragma region Enum Section
private:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Enums, meta = (AllowPrivateAccess = "true"))
	EActorType ActorType;
#pragma endregion

public:
	bool bIsOpenedSanctum;

	UPROPERTY(EditAnywhere, Category = "Widget")
	TSubclassOf<UValhallaSanctumHUD> SanctumHUD;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Widget")
	UValhallaSanctumHUD* SanctumWidget;

public:
	UFUNCTION()
	void OpenSanctum();

	UFUNCTION()
	void CloseSanctum();

	UFUNCTION(BlueprintCallable)
	virtual EActorType& GetActorTypeTag() override;

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Widget")
	bool bIsSanctumAlreadyMade;
};
