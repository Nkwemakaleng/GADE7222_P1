#pragma once

#include "CoreMinimal.h"
#include "WidgetBase.h"
#include "HUDWidget.generated.h"

class UTextBlock;
class UStatBarBase;
class UCombatComponent;

UCLASS(Abstract, Blueprintable)
class GADE7222_P1_API UHUDWidget : public UWidgetBase
{
	GENERATED_BODY()

public:
	// Called by the PlayerCharacter to hook up the HUD to the combat system
	UFUNCTION(BlueprintCallable, Category = "HUD")
	void SetCombatComponent(UCombatComponent* InCombatComponent);

protected:
	virtual void NativeConstruct() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

	// --- BindWidget: names must match in the Blueprint designer ---

	// Text showing "Weapon Name" e.g. "Shotriser"
	UPROPERTY(meta = (BindWidgetOptional))
	UTextBlock* Text_WeaponName;

	// Text showing "12" (current ammo pool)
	UPROPERTY(meta = (BindWidgetOptional))
	UTextBlock* Text_CurrentAmmo;

	// Text showing "30" (max ammo pool)
	UPROPERTY(meta = (BindWidgetOptional))
	UTextBlock* Text_MaxAmmo;

	// Optional stat bar to visually represent the ammo pool
	UPROPERTY(meta = (BindWidgetOptional))
	UStatBarBase* StatBar_Ammo;

	// Crosshair image (just for visual reference in BP, no logic needed)
	

private:
	UPROPERTY()
	UCombatComponent* CachedCombatComponent;

	// Track last values so we only update the UI when they change
	int32 LastAmmo = -1;
	int32 LastMaxAmmo = -1;
	FString LastWeaponName;
};