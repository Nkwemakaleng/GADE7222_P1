#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "DamageableInterface.generated.h"

UINTERFACE(MinimalAPI, Blueprintable)
class UDamageableInterface : public UInterface
{
	GENERATED_BODY()
};

class GADE7222_P1_API IDamageableInterface
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Damage")
	void ReceiveDamage(float DamageAmount, AActor* DamageCauser, const FVector& HitLocation);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Damage")
	float GetCurrentHealth() const;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Damage")
	bool IsAlive() const;
};