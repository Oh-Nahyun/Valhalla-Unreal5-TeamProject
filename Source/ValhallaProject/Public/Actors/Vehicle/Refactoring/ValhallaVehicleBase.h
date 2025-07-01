// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "WheeledVehiclePawn.h"
#include "Others/Enums.h"
#include "Interface/TeamInterface.h"
#include "Interface/ActorTypeInterface.h"
#include "Interface/VehicleInterface.h"
#include "ValhallaVehicleBase.generated.h"

DECLARE_DELEGATE_ThreeParams(FOnVehicleHealthChangedSignature, float, float, AController*);

class USpringArmComponent;
class UCameraComponent;
class UWidgetComponent;
class UInputAction;
class UInputMappingContext;
struct FInputActionValue;
class UChildActorComponent;
class AValhallaPlayerController;
class AVehicleSecondaryPawn;
class USoundBase;

/**
 * 
 */
UCLASS()
class VALHALLAPROJECT_API AValhallaVehicleBase : public AWheeledVehiclePawn, public ITeamInterface, public IActorTypeInterface, public IVehicleInterface
{
	GENERATED_BODY()
	
public:
	AValhallaVehicleBase();

protected:
	virtual void BeginPlay() override;

	virtual void Tick(float DeltaTime) override;
	virtual void PossessedBy(AController* NewController) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;
	virtual void Destroyed() override;
	virtual void UnPossessed() override;

protected:
	UFUNCTION(Client, Reliable)
	void Client_GetSubsystem(AController* InController);

private:
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Vehicle, meta = (AllowPrivateAccess = "true"), Replicated)
	TObjectPtr<AActor> PrimarySeat;

public:
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Vehicle, meta = (AllowPrivateAccess = "true"), Replicated)
	TObjectPtr<AActor> SecondarySeat;

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Controller, meta = (AllowPrivateAccess = "true"), Replicated)
	TObjectPtr<AValhallaPlayerController> VehiclePlayerController;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Secondary, meta = (AllowPrivateAccess = "true"), Replicated)
	TObjectPtr<AVehicleSecondaryPawn> VehicleSecondaryPawn;


#pragma region Component Section
private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Components, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<USpringArmComponent> CameraBoom;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Components, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UCameraComponent> FollowCamera;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Component, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UWidgetComponent> VehicleWidgetComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Component, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<USceneComponent> PrimarySeatGetOffLocation;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Component, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<USceneComponent> SecondarySeatGetOffLocation;

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Component, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UStaticMeshComponent> AttackPartMesh;

private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Component, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<USceneComponent> TurretDetectLocation;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Component, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UChildActorComponent> SecondaryPawn;

public:
	FORCEINLINE USceneComponent* GetSecondarySeatGetOffLocation() { return SecondarySeatGetOffLocation; }

private:
	UFUNCTION()
	virtual void OnMeshBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
#pragma endregion


#pragma region Input Section
private:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UInputMappingContext> VehiclePrimaryMappingContext;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UInputAction> VehicleLookAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UInputAction> AttackAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UInputAction> GetOffAction;

	void VehicleLook(const FInputActionValue& Value);
	void VehicleAttack(const FInputActionValue& Value);
	void VehicleGetOff(const FInputActionValue& Value);

protected:
	UFUNCTION(BlueprintImplementableEvent)
	void Attack();
#pragma endregion


#pragma region Enum Section
protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Enums, meta = (AllowPrivateAccess = "true"), Replicated)
	EPlayerTeamType TeamType;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Enums, meta = (AllowPrivateAccess = "true"))
	EActorType ActorType;

public:
	FORCEINLINE const EPlayerTeamType& GetTeamType() { return TeamType; }
	FORCEINLINE const EActorType& GetActorType() { return ActorType; }

	FORCEINLINE void SetTeamType(EPlayerTeamType InNewTeamType) { TeamType = InNewTeamType; }
#pragma endregion


#pragma region Status Section
protected:
	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = Status, meta = (AllowPrivateAccess = "true"), Replicated)
	float CurrentHealth;

	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = Status, meta = (AllowPrivateAccess = "true"))
	float MaxHealth;

	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = Status, meta = (AllowPrivateAccess = "true"))
	float SiegeWeaponDefense;

	UPROPERTY(EditAnyWhere, BlueprintReadOnly, Category = Status, meta = (AllowPrivateAccess = "true"))
	int32 GoldGivenAmount;

private:
	void InitializeStatus();
#pragma endregion


#pragma region Delegate Section
public:
	FOnVehicleHealthChangedSignature OnVehicleHealthChangedDelegate;

	UFUNCTION(NetMulticast, Reliable)
	void OnVehicleHealthChanged(float InCurrentHealth, float InMaxHealth, AController* InInstigatorController);
#pragma endregion


#pragma region Interface Section
public:
	virtual EPlayerTeamType& GetActorTeamType() override { return TeamType; }
	
	UFUNCTION(BlueprintCallable)
	virtual EActorType& GetActorTypeTag() override { return ActorType; }

	virtual ESeatType GetSeat() override;
	virtual USkeletalMeshComponent* GetVehicleMesh() override { return GetMesh(); }

	// ∏Æ∆—≈‰∏µ
	virtual void SetVehicleSeat(AActor* InActor, const ESeatType& InSeatType) override;
	virtual AActor* GetSecondaryPawn() override;
#pragma endregion


#pragma region GetOff Section
	UFUNCTION(Server, Reliable)
	void Server_VehicleGetOff();

	UFUNCTION(NetMulticast, Reliable)
	void MC_VehicleGetOff();
#pragma endregion


#pragma region Destroy Section
private:
	void DestroyVehicle();
#pragma endregion


#pragma region Attack Section
protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Attack, meta = (AllowPrivateAccess = "true"))
	float AttackDamage;

	UFUNCTION(BlueprintCallable)
	void ApplyAttackDamage(AActor* InActor);

	UFUNCTION(BlueprintPure)
	bool IsStopNow();

	UFUNCTION(BlueprintImplementableEvent)
	void ServerHitEffect(EActorType InActorType);
#pragma endregion


#pragma region Skill Section
private:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Attack, meta = (AllowPrivateAccess = "true"))
	TArray<FName> SkillRightLocations;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Attack, meta = (AllowPrivateAccess = "true"))
	TArray<FName> SkillLeftLocations;

	UFUNCTION(BlueprintCallable)
	void ShotSkill();

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Attack, meta = (AllowPrivateAccess = "true"))
	TSubclassOf<class AVehicleSkillProjectile> SkillProjectileClass;
#pragma endregion


#pragma region Sound Section
private:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Sound, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<USoundBase> DamagedSound;

	UFUNCTION(NetMulticast, Unreliable)
	void PlayVehicleDamagedSound();
#pragma endregion
};
