#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "StaminaComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnStaminaChangedSignature, float, CurrentStamina, float, MaxStamina);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class GADE7222_P1_API UStaminaComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UStaminaComponent();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stamina")
	float MaxStamina = 100.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stamina")
	float DrainRate = 20.0f;      // per second while sprinting

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stamina")
	float RegenRate = 15.0f;      // per second when idle

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stamina")
	float RegenDelay = 1.5f;      // seconds after stopping sprint before regen kicks in

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stamina")
	float MinToSprint = 10.0f;    // must have this much to start sprinting again

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Stamina")
	float CurrentStamina = 100.0f;

	UPROPERTY(BlueprintAssignable, Category = "Stamina")
	FOnStaminaChangedSignature OnStaminaChanged;

	UFUNCTION(BlueprintCallable, Category = "Stamina")
	void DrainStamina(float DeltaTime);

	UFUNCTION(BlueprintCallable, Category = "Stamina")
	void RegenerateStamina(float DeltaTime);

	UFUNCTION(BlueprintCallable, Category = "Stamina")
	void ConsumeStamina(float Amount);

	UFUNCTION(BlueprintCallable, Category = "Stamina")
	void RestoreStamina(float Amount);

	UFUNCTION(BlueprintPure, Category = "Stamina")
	bool CanStartSprint() const { return CurrentStamina >= MinToSprint; }

	UFUNCTION(BlueprintPure, Category = "Stamina")
	bool HasStamina() const { return CurrentStamina > 0.0f; }

	UFUNCTION(BlueprintPure, Category = "Stamina")
	float GetStaminaPercent() const { return MaxStamina > 0.0f ? CurrentStamina / MaxStamina : 0.0f; }

protected:
	virtual void BeginPlay() override;
};