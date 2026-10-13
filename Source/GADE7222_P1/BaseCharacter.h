#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "DamageableInterface.h"
#include "BaseCharacter.generated.h"

class UCombatComponent;
class UHealthComponent;
class UStaminaComponent;
class AWeaponBase;

UCLASS(Abstract)
class GADE7222_P1_API ABaseCharacter : public ACharacter, public IDamageableInterface
{
	GENERATED_BODY()

public:
	ABaseCharacter();

	// --- COMPONENTS ---
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat")
	UCombatComponent* CombatComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Health")
	UHealthComponent* HealthComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Stamina")
	UStaminaComponent* StaminaComponent;

	// --- SPRINT ---
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	float WalkSpeed = 600.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	float SprintSpeed = 950.0f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Movement")
	bool bIsSprinting = false;

	UFUNCTION(BlueprintCallable, Category = "Movement")
	void StartSprint();

	UFUNCTION(BlueprintCallable, Category = "Movement")
	void StopSprint();

	// --- INTERACTION ---
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interaction")
	float InteractionRange = 300.0f;

	// The currently-focused interactable (updated by trace)
	UPROPERTY(BlueprintReadOnly, Category = "Interaction")
	AActor* FocusedInteractable;

	UFUNCTION(BlueprintCallable, Category = "Interaction")
	void TryInteract();

	UFUNCTION(BlueprintCallable, Category = "Interaction")
	void UpdateInteractionFocus();

	// --- COMBAT WRAPPERS ---
	UFUNCTION(BlueprintCallable, Category = "Combat")
	void StartFire();
	UFUNCTION(BlueprintCallable, Category = "Combat")
	void StopFire();
	UFUNCTION(BlueprintCallable, Category = "Combat")
	void NextWeapon();
	UFUNCTION(BlueprintCallable, Category = "Combat")
	void PickupWeapon(TSubclassOf<AWeaponBase> WeaponClass, int32 AmmoAmount);

	// --- IDamageableInterface ---
	virtual void ReceiveDamage_Implementation(float DamageAmount, AActor* DamageCauser, const FVector& HitLocation) override;
	virtual float GetCurrentHealth_Implementation() const override;
	virtual bool IsAlive_Implementation() const override;

	// --- MOVEMENT  ---
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	float MaxWalkSpeedCrouched = 300.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	float JumpHeight = 400.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	float AirControl = 0.8f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	float GroundFriction = 8.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	float BrakingDecelerationWalking = 2048.0f;

	// Starting weapons (set in BP)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat|Loadout")
	TArray<TSubclassOf<AWeaponBase>> StartingWeapons;

	virtual void MoveForward(float Value);
	virtual void MoveRight(float Value);
	virtual void StartJump();
	virtual void StopJump();
	virtual void StartCrouch();
	virtual void StopCrouch();

	//---CAMERA---
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
	class UCameraComponent* PlayerCamera;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
	class USpringArmComponent* CameraBoom;
	
	//---RESPAWN---
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Respawn")
	float RespawnDelay = 3.0f;

	UFUNCTION(BlueprintCallable, Category = "Respawn")
	void Respawn();

	UFUNCTION(BlueprintCallable, Category = "Respawn")
	AActor* FindRandomRespawnPoint() const;
protected:
	virtual void BeginPlay() override;
	void Tick(float DeltaTime);

	// handles stamina drain/regen and sprint auto-stop
	void TickSprint(float DeltaTime);

	FTimerHandle RegenDelayTimer;
	bool bCanRegenStamina = false;
	
	//Handles disabling controls and respawning
	UFUNCTION()
	void HandleDeath(AActor* Killer);

	FTimerHandle RespawnTimerHandle;
};