// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "WheeledVehiclePawn.h"
#include "Interface/TeamInterface.h"
#include "Interface/ActorTypeInterface.h"
#include "GenericTeamAgentInterface.h"
#include "Interface/VehicleInterface.h"
#include "Perception/AISightTargetInterface.h"
#include "Others/Enums.h"
#include "ValhallaSiegeWeapon.generated.h"

DECLARE_DELEGATE_ThreeParams(FOnVehicleHealthChangedSignature, float, float, AController*);

class USpringArmComponent;
class UCameraComponent;
class UWidgetComponent;
class ACannonBullet;
class UInputAction;
class UInputMappingContext;
struct FInputActionValue;

/**
 * 
 */
UCLASS()
class VALHALLAPROJECT_API AValhallaSiegeWeapon : public AWheeledVehiclePawn, public ITeamInterface, public IActorTypeInterface, public IGenericTeamAgentInterface, public IVehicleInterface, public IAISightTargetInterface
{
	GENERATED_BODY()
	
public:
	AValhallaSiegeWeapon();

protected:
	virtual void BeginPlay() override;
	// Network
	virtual void PossessedBy(AController* NewController) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	// Damage
	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;
	virtual void Destroyed() override;


protected:
	UFUNCTION(Client, Reliable)
	void Client_GetSubsystem(AController* InController);

private:
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Vehicle, meta = (AllowPrivateAccess = "true"), Replicated)
	TObjectPtr<AActor> PrimarySeat;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Vehicle, meta = (AllowPrivateAccess = "true"), Replicated)
	TObjectPtr<AActor> SecondarySeat;


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

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Component, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UStaticMeshComponent> AttackPartMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Component, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<USceneComponent> BulletShotLocation;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Component, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<USceneComponent> TurretDetectLocation;

private:
	UFUNCTION()
	virtual void OnMeshBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
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


#pragma region Input Section
private:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UInputMappingContext> VehicleMappingContext;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UInputAction> VehicleMoveForwardAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UInputAction> VehicleTurnRightAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UInputAction> VehicleLookAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UInputAction> GetOffAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UInputAction> AttackAction;

	void VehicleMoveForwardTriggered(const FInputActionValue& Value);
	void VehicleMoveForwardStarted(const FInputActionValue& Value);
	void VehicleTurnRightTriggered(const FInputActionValue& Value);
	void VehicleTurnRightStarted(const FInputActionValue& Value);
	void VehicleTurnRightCompleted(const FInputActionValue& Value);
	void VehicleLook(const FInputActionValue& Value);
	void VehicleAttack(const FInputActionValue& Value);

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Input, meta = (AllowPrivateAccess = "true"), Replicated)
	uint8 IsAttackingNow : 1;

	UFUNCTION(BlueprintImplementableEvent)
	void Attack();
#pragma endregion


#pragma region GetOff Section
	void GetOff(const FInputActionValue& Value);

	UFUNCTION(Server, Reliable)
	void Server_GetOff(APawn* InNewPawn, AController* InController);

	UFUNCTION(NetMulticast, Reliable)
	void MC_GetOff(APawn* InNewPawn);

	UFUNCTION(BlueprintImplementableEvent)
	void ResetPerception();
#pragma endregion


#pragma region Interface Section
public:
	virtual EPlayerTeamType& GetActorTeamType() override;

	UFUNCTION(BlueprintCallable)
	virtual EActorType& GetActorTypeTag() override;

	virtual ESeatType GetSeat() override;
	virtual USkeletalMeshComponent* GetVehicleMesh() override;

	virtual FGenericTeamId GetGenericTeamId() const override { return FGenericTeamId((uint8)TeamType); }

	virtual UAISense_Sight::EVisibilityResult CanBeSeenFrom(const FCanBeSeenFromContext& Context, FVector& OutSeenLocation, int32& OutNumberOfLoSChecksPerformed, int32& OutNumberOfAsyncLosCheckRequested, float& OutSightStrength, int32* UserData = nullptr, const FOnPendingVisibilityQueryProcessedDelegate* Delegate = nullptr) override;

	virtual void SetVehicleSeat(AActor* InActor, const ESeatType& InSeatType) override;
	virtual AActor* GetSecondaryPawn() override;
#pragma endregion


#pragma region Status Section
protected:
	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = Status, meta = (AllowPrivateAccess = "true"), ReplicatedUsing = OnRep_CurrentHealth)
	float CurrentHealth;

	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = Status, meta = (AllowPrivateAccess = "true"))
	float MaxHealth;

	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = Status, meta = (AllowPrivateAccess = "true"))
	float SiegeWeaponDefense;

	UPROPERTY(EditAnyWhere, BlueprintReadOnly, Category = Status, meta = (AllowPrivateAccess = "true"))
	int32 GoldGivenAmount;

public:
	UFUNCTION()
	void OnRep_CurrentHealth();

private:
	void InitializeStatus();
#pragma endregion


#pragma region Delegate Section
public:
	FOnVehicleHealthChangedSignature OnVehicleHealthChangedDelegate;

	UFUNCTION(NetMulticast, Reliable)
	void OnVehicleHealthChanged(float InCurrentHealth, float InMaxHealth, AController* InInstigatorController);
#pragma endregion


#pragma region Destroy Section
private:
	void DestroySiegeWeapon();
#pragma endregion


#pragma region Attack Section
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Attack, meta = (AllowPrivateAccess = "true"))
	float AttackDamage;

	void ApplyAttackDamage(AActor* InActor);

	UFUNCTION(Server, Reliable)
	void Server_ApplyAttackDamage(AActor* InActor);

	UFUNCTION(BlueprintCallable)
	void CannonAttack();

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Fire, meta = (AllowPrivateAccess = "true"))
	TSubclassOf<ACannonBullet> CannonBulletClass;
#pragma endregion
};
