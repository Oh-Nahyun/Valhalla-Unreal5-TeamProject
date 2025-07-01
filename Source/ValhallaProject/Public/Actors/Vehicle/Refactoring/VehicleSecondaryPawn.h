// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "Others/Enums.h"
#include "Interface/ActorTypeInterface.h"
#include "Interface/TeamInterface.h"
#include "VehicleSecondaryPawn.generated.h"

class USpringArmComponent;
class UCameraComponent;
class UInputAction;
class UInputMappingContext;
struct FInputActionValue;
class USkeletalMeshComponent;
class AValhallaPlayerController;
class AValhallaVehicleBase;

UCLASS()
class VALHALLAPROJECT_API AVehicleSecondaryPawn : public APawn, public IActorTypeInterface, public ITeamInterface
{
	GENERATED_BODY()

public:
	AVehicleSecondaryPawn();

protected:
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaSeconds) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	virtual void PossessedBy(AController* NewController) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;


protected:
	UFUNCTION(Client, Reliable)
	void Client_GetSubsystem(AController* InController);

private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Controller, meta = (AllowPrivateAccess = "true"), Replicated)
	TObjectPtr<AValhallaPlayerController> VehiclePlayerController;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Vehicle, meta = (AllowPrivateAccess = "true"), Replicated)
	TObjectPtr<AValhallaVehicleBase> ControlledVehicle;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Enums, meta = (AllowPrivateAccess = "true"), Replicated)
	EPlayerTeamType TeamType;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Enums, meta = (AllowPrivateAccess = "true"))
	EActorType ActorType;

public:
	FORCEINLINE void SetTeamType(EPlayerTeamType InNewTeamType) { TeamType = InNewTeamType; }


#pragma region Component Section
private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Components, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<USpringArmComponent> CameraBoom;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Components, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UCameraComponent> FollowCamera;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = Components, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<USkeletalMeshComponent> Mesh;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = Components, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UStaticMeshComponent> BallistaMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Component, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<USceneComponent> ShotLocation;

public:
	USkeletalMeshComponent* GetMesh() const { return Mesh; }
#pragma endregion


#pragma region Input Section
private:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UInputMappingContext> VehicleSecondaryMappingContext;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UInputAction> LookAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UInputAction> SkillAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UInputAction> GetOffAction;

	void SecondaryPawnLook(const FInputActionValue& Value);
	void SecondaryGetOff(const FInputActionValue& Value);

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Input, meta = (AllowPrivateAccess = "true"), Replicated)
	uint8 IsSkillNow : 1;
#pragma endregion


#pragma region GetOff Section
	UFUNCTION(Server, Reliable)
	void Server_SecondaryGetOff();

	UFUNCTION(NetMulticast, Reliable)
	void MC_SecondaryGetOff();
#pragma endregion


private:
	UFUNCTION(BlueprintCallable)
	void ShotBallista();


#pragma region Interface Section
public:
	virtual EPlayerTeamType& GetActorTeamType() override { return TeamType; }

	UFUNCTION(BlueprintCallable)
	virtual EActorType& GetActorTypeTag() override { return ActorType; };
#pragma endregion
};
