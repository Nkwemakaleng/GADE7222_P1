#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "BaseCharacter.generated.h"

class UCombatComponent;
class AWeaponBase;

UCLASS(Abstract)
class GADE7222_P1_API ABaseCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	ABaseCharacter();

protected:
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;

	// --- COMBAT COMPONENT (Accessible by AI and Player) ---
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat")
	UCombatComponent* CombatComponent;

	// --- COMBAT ACTIONS ---
	UFUNCTION(BlueprintCallable, Category = "Combat")
	void StartFire();

	UFUNCTION(BlueprintCallable, Category = "Combat")
	void StopFire();

	UFUNCTION(BlueprintCallable, Category = "Combat")
	void NextWeapon();

	UFUNCTION(BlueprintCallable, Category = "Combat")
	void PickupWeapon(TSubclassOf<AWeaponBase> WeaponClass, int32 AmmoAmount);

	// --- MOVEMENT PROPERTIES ---
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	float MaxWalkSpeed = 600.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	float MaxWalkSpeedCrouched = 300.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	float JumpHeight = 400.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	float AirControl = 0.8f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	float GroundFriction = 8.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	float BrakingDecelerationWalking = 500.0f;

	// Movement functions
	virtual void MoveForward(float Value);
	virtual void MoveRight(float Value);
	virtual void StartJump();
	virtual void StopJump();
	virtual void StartCrouch();
	virtual void StopCrouch();

	// Camera
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
	class UCameraComponent* PlayerCamera;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
	class USpringArmComponent* CameraBoom;
};