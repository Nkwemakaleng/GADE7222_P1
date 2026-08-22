// BaseCharacter.h
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "BaseCharacter.generated.h"

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

	// Movement properties
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
	UFUNCTION(BlueprintCallable, Category = "Movement")
	virtual void MoveForward(float Value);

	UFUNCTION(BlueprintCallable, Category = "Movement")
	virtual void MoveRight(float Value);

	UFUNCTION(BlueprintCallable, Category = "Movement")
	virtual void StartJump();

	UFUNCTION(BlueprintCallable, Category = "Movement")
	virtual void StopJump();

	UFUNCTION(BlueprintCallable, Category = "Movement")
	virtual void StartCrouch();

	UFUNCTION(BlueprintCallable, Category = "Movement")
	virtual void StopCrouch();

	// Camera
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
	class UCameraComponent* PlayerCamera;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
	class USpringArmComponent* CameraBoom;

	//debugging
	UFUNCTION(BlueprintCallable, Category = "Debug")
	void LogMovementStatus();
};