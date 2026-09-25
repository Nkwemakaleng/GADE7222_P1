#include "HUDWidget.h"
#include "BaseCharacter.h"
#include "CombatComponent.h"
#include "HealthComponent.h"
#include "InteractableInterface.h"
#include "StaminaComponent.h"
#include "Components/TextBlock.h"
#include "StatBarBase.h"

void UHUDWidget::NativeConstruct()
{
	Super::NativeConstruct();

	//Changes Colours of the Statbars
	if (StatBar_Ammo)
	{
		StatBar_Ammo->SetBarColor(FLinearColor(0.0f, 0.0f, 1.0f, 1.0f)); //Blue
	}
	if (StatBar_Health)
	{
		StatBar_Health->SetBarColor(FLinearColor(1.0f, 0.0f, 0.0f, 1.0f));//Red
	}
	if (StatBar_Stamina)
	{
		StatBar_Stamina->SetBarColor(FLinearColor(1.0f, 0.85f, 0.0f, 1.0f));//Yellow
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

	// Update ammo bar 
	/*if (StatBar_Ammo)
	{
		StatBar_Ammo->SetStatValues((float)CurrentAmmo, (float)MaxAmmo);
	}*/
	//Update all bars if necessary
	if (TrackedCharacter)
	{
		//Weapon 
		if (TrackedCharacter->CombatComponent && StatBar_Ammo)
		{
			StatBar_Ammo->SetStatValues(TrackedCharacter->CombatComponent->GetCurrentAmmo(),TrackedCharacter->CombatComponent->GetMaxAmmo());
		}
		// Health
		if (TrackedCharacter->HealthComponent && StatBar_Health)
		{
			StatBar_Health->SetStatValues(
				TrackedCharacter->HealthComponent->CurrentHealth,
				TrackedCharacter->HealthComponent->MaxHealth);
		}

		// Stamina
		if (TrackedCharacter->StaminaComponent && StatBar_Stamina)
		{
			StatBar_Stamina->SetStatValues(
				TrackedCharacter->StaminaComponent->CurrentStamina,
				TrackedCharacter->StaminaComponent->MaxStamina);
		}

		// Interact prompt
		if (Text_InteractPrompt)
		{
			ABaseCharacter* Char = TrackedCharacter;
			Char->UpdateInteractionFocus();

			if (Char->FocusedInteractable &&
				Char->FocusedInteractable->GetClass()->ImplementsInterface(UInteractableInterface::StaticClass()))
			{
				FText Prompt = IInteractableInterface::Execute_GetInteractPrompt(Char->FocusedInteractable);
				Text_InteractPrompt->SetText(Prompt);
				Text_InteractPrompt->SetVisibility(ESlateVisibility::Visible);
			}
			else
			{
				Text_InteractPrompt->SetVisibility(ESlateVisibility::Hidden);
			}
		}
		
	}
	
	
}
void UHUDWidget::SetTrackedCharacter(ABaseCharacter* InCharacter)
{
	TrackedCharacter = InCharacter;
}