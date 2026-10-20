#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "HealthBarWidget.generated.h"

class UProgressBar;
class UHealthComponent;
class UTextBlock;

UCLASS(Abstract, Blueprintable)
class GADE7222_P1_API UHealthBarWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	// Call this to link the UI to the health data
	UFUNCTION(BlueprintCallable, Category = "Health")
	void SetHealthComponent(UHealthComponent* InHealthComponent);

protected:
	virtual void NativeConstruct() override;

	// The progress bar in the Blueprint 
	UPROPERTY(meta = (BindWidgetOptional))
	UProgressBar* HealthBar;
	//The text block in the blueprint
	UPROPERTY(meta = (BindWidgetOptional))
	UTextBlock* Text_HealthValue;

	UFUNCTION()
	void UpdateHealth(float CurrentHealth, float MaxHealth);

private:
	UPROPERTY()
	UHealthComponent* CachedHealthComponent;
};