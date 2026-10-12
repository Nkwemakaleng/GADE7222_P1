#pragma once

#include "CoreMinimal.h"
#include "WidgetBase.h"
#include "HUDWidget.generated.h"

class UTextBlock;
class UStatBarBase;
class UCombatComponent;
class UHealthComponent;
class UStaminaComponent;
class ABaseCharacter;

UCLASS(Abstract, Blueprintable)
class GADE7222_P1_API UHUDWidget : public UWidgetBase
{
	GENERATED_BODY()

public:
	// Bind to any character's components by listening — no permanent pointers
	UFUNCTION(BlueprintCallable, Category = "HUD")
	void BindToCharacter(ABaseCharacter* InCharacter);

	UFUNCTION(BlueprintCallable, Category = "HUD")
	void UnbindFromCharacter();

protected:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

	// --- Widget bindings ---
	UPROPERTY(meta = (BindWidgetOptional)) UTextBlock* Text_WeaponName;
	UPROPERTY(meta = (BindWidgetOptional)) UTextBlock* Text_CurrentAmmo;
	UPROPERTY(meta = (BindWidgetOptional)) UTextBlock* Text_MaxAmmo;
	UPROPERTY(meta = (BindWidgetOptional)) UTextBlock* Text_InteractPrompt;
	UPROPERTY(meta = (BindWidgetOptional)) UStatBarBase* StatBar_Ammo;
	UPROPERTY(meta = (BindWidgetOptional)) UStatBarBase* StatBar_Health;
	UPROPERTY(meta = (BindWidgetOptional)) UStatBarBase* StatBar_Stamina;

private:
	// Delegate handlers — receive values, not the source object
	UFUNCTION() void HandleAmmoChanged(int32 CurrentAmmo, int32 MaxAmmo);
	UFUNCTION() void HandleWeaponChanged(const FString& WeaponName);
	UFUNCTION() void HandleHealthChanged(float Current, float Max);
	UFUNCTION() void HandleStaminaChanged(float Current, float Max);

	// Weak refs so we don't prevent GC
	UPROPERTY() TWeakObjectPtr<UCombatComponent> CachedCombat;
	UPROPERTY() TWeakObjectPtr<UHealthComponent> CachedHealth;
	UPROPERTY() TWeakObjectPtr<UStaminaComponent> CachedStamina;
	UPROPERTY() TWeakObjectPtr<ABaseCharacter> CachedCharacter;
};