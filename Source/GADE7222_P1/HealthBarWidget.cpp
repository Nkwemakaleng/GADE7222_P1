#include "HealthBarWidget.h"
#include "HealthComponent.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"

void UHealthBarWidget::NativeConstruct()
{
	Super::NativeConstruct();
	if (HealthBar)
	{
		HealthBar->SetFillColorAndOpacity(FLinearColor::Red);
	}
	if (Text_HealthValue)
	{
		// Make the text white and add a shadow for readability
		Text_HealthValue->SetColorAndOpacity(FSlateColor(FLinearColor::White));
		Text_HealthValue->SetShadowOffset(FVector2D(1.f, 1.f));
		Text_HealthValue->SetShadowColorAndOpacity(FLinearColor::Black);
	}
}

void UHealthBarWidget::SetHealthComponent(UHealthComponent* InHealthComponent)
{
	if (CachedHealthComponent)
	{
		CachedHealthComponent->OnHealthChanged.RemoveDynamic(this, &UHealthBarWidget::UpdateHealth);
	}
	

	CachedHealthComponent = InHealthComponent;

	if (CachedHealthComponent)
	{
		CachedHealthComponent->OnHealthChanged.AddDynamic(this, &UHealthBarWidget::UpdateHealth);
		// Set initial value
		UpdateHealth(CachedHealthComponent->CurrentHealth, CachedHealthComponent->MaxHealth);
	}
}


void UHealthBarWidget::UpdateHealth(float CurrentHealth, float MaxHealth)
{
	//Update the bar
	if (HealthBar && MaxHealth > 0.f)
	{
		HealthBar->SetPercent(CurrentHealth / MaxHealth);
	}
	if (Text_HealthValue)
	{
		FString HealthString = FString::Printf(TEXT("%d / %d"), FMath::CeilToInt(CurrentHealth), FMath::CeilToInt(MaxHealth));
		Text_HealthValue->SetText(FText::FromString(HealthString));
	}
}