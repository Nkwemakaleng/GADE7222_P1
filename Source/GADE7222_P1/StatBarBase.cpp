#include "StatBarBase.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"

void UStatBarBase::NativeConstruct()
{
	Super::NativeConstruct();

	if (ProgressBar_Stat)
	{
		CurrentPercent = ProgressBar_Stat->GetPercent();
		TargetPercent = CurrentPercent;
	}
}

void UStatBarBase::SetStatValues(float CurrentValue, float MaxValue)
{
	if (MaxValue <= 0.0f)
	{
		TargetPercent = 0.0f;
		return;
	}

	TargetPercent = FMath::Clamp(CurrentValue / MaxValue, 0.0f, 1.0f);

	if (!bUseSmoothInterp && ProgressBar_Stat)
	{
		CurrentPercent = TargetPercent;
		ProgressBar_Stat->SetPercent(CurrentPercent);
	}
	if (Text_StatValue)
	{
		FString ValueString = FString::Printf(TEXT("%d / %d"), FMath::RoundToInt(CurrentValue), FMath::RoundToInt(MaxValue));
		Text_StatValue->SetText(FText::FromString(ValueString));
	}
}

void UStatBarBase::SetBarColor(FLinearColor NewColor)
{
	if (ProgressBar_Stat)
	{
		ProgressBar_Stat->SetFillColorAndOpacity(NewColor);
	}
}

void UStatBarBase::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	if (!ProgressBar_Stat || !bUseSmoothInterp) return;

	// Smoothly interpolate toward the target
	CurrentPercent = FMath::FInterpTo(CurrentPercent, TargetPercent, InDeltaTime, InterpSpeed);
	ProgressBar_Stat->SetPercent(CurrentPercent);
}