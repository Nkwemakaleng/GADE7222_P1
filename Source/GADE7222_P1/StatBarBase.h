#pragma once

#include "CoreMinimal.h"
#include "WidgetBase.h"
#include "StatBarBase.generated.h"

class UProgressBar;

UCLASS(Abstract, Blueprintable)
class GADE7222_P1_API UStatBarBase : public UWidgetBase
{
	GENERATED_BODY()

public:
	// Call this from code to update the bar (current, max)
	UFUNCTION(BlueprintCallable, Category = "Stat Bar")
	void SetStatValues(float CurrentValue, float MaxValue);

	// Call this to set the bar's color (e.g., red for health, yellow for ammo)
	UFUNCTION(BlueprintCallable, Category = "Stat Bar")
	void SetBarColor(FLinearColor NewColor);

	// Smoothly interpolate toward the target percentage each tick
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stat Bar")
	float InterpSpeed = 8.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stat Bar")
	bool bUseSmoothInterp = true;

protected:
	virtual void NativeConstruct() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

	
	UPROPERTY(meta = (BindWidget))
	UProgressBar* ProgressBar_Stat;
	
	UPROPERTY(meta = (BindWidgetOptional))
	class UTextBlock* Text_StatValue;

private:
	float TargetPercent = 1.0f;
	float CurrentPercent = 1.0f;
};