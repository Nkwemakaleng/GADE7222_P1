#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "HealthComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnHealthChangedSignature, float, CurrentHealth, float, MaxHealth);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnDeathSignature, AActor*, Killer);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class GADE7222_P1_API UHealthComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UHealthComponent();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Health")
	float MaxHealth = 100.0f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Health")
	float CurrentHealth = 100.0f;

	UPROPERTY(BlueprintAssignable, Category = "Health")
	FOnHealthChangedSignature OnHealthChanged;

	UPROPERTY(BlueprintAssignable, Category = "Health")
	FOnDeathSignature OnDeath;

	UFUNCTION(BlueprintCallable, Category = "Health")
	void ApplyDamage(float Amount, AActor* DamageCauser);

	UFUNCTION(BlueprintCallable, Category = "Health")
	void Heal(float Amount);

	UFUNCTION(BlueprintCallable, Category = "Health")
	void ResetHealth();

	UFUNCTION(BlueprintPure, Category = "Health")
	float GetHealthPercent() const;

	UFUNCTION(BlueprintPure, Category = "Health")
	bool IsDead() const { return CurrentHealth <= 0.0f; }

protected:
	virtual void BeginPlay() override;
};