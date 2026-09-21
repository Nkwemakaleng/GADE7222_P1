#include "HUDWidget.h"
#include "CombatComponent.h"
#include "Components/TextBlock.h"
#include "StatBarBase.h"

void UHUDWidget::NativeConstruct()
{
	Super::NativeConstruct();

	// Default crosshair-relevant setup can go here
	if (StatBar_Ammo)
	{
		StatBar_Ammo->SetBarColor(FLinearColor(1.0f, 0.85f, 0.0f, 1.0f)); // Yellow
	}
}

void UHUDWidget::SetCombatComponent(UCombatComponent* InCombatComponent)
{
	CachedCombatComponent = InCombatComponent;

	// Force a fresh update next tick
	LastAmmo = -1;
	LastMaxAmmo = -1;
	LastWeaponName.Empty();
}

void UHUDWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	if (!CachedCombatComponent) return;

	// --- Pull fresh values from the combat component ---
	int32 CurrentAmmo = CachedCombatComponent->GetCurrentAmmo();
	int32 MaxAmmo = CachedCombatComponent->GetMaxAmmo();
	FString WeaponName = CachedCombatComponent->GetCurrentWeaponName();

	// --- Only update text when the value actually changed (avoids layout thrash) ---
	if (CurrentAmmo != LastAmmo && Text_CurrentAmmo)
	{
		Text_CurrentAmmo->SetText(FText::AsNumber(CurrentAmmo));
		LastAmmo = CurrentAmmo;
	}

	if (MaxAmmo != LastMaxAmmo && Text_MaxAmmo)
	{
		Text_MaxAmmo->SetText(FText::AsNumber(MaxAmmo));
		LastMaxAmmo = MaxAmmo;
	}

	if (WeaponName != LastWeaponName && Text_WeaponName)
	{
		Text_WeaponName->SetText(FText::FromString(WeaponName));
		LastWeaponName = WeaponName;
	}

	// --- Update ammo bar ---
	if (StatBar_Ammo)
	{
		StatBar_Ammo->SetStatValues((float)CurrentAmmo, (float)MaxAmmo);
	}
}