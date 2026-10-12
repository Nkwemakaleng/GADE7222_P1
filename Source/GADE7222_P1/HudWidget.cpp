#include "HUDWidget.h"
#include "CombatComponent.h"
#include "HealthComponent.h"
#include "StaminaComponent.h"
#include "BaseCharacter.h"
#include "InteractableInterface.h"
#include "Components/TextBlock.h"
#include "StatBarBase.h"

void UHUDWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (StatBar_Ammo)    StatBar_Ammo->SetBarColor(FLinearColor(1.0f, 0.85f, 0.0f, 1.0f));
	if (StatBar_Health)  StatBar_Health->SetBarColor(FLinearColor(0.85f, 0.1f, 0.1f, 1.0f));
	if (StatBar_Stamina) StatBar_Stamina->SetBarColor(FLinearColor(0.1f, 0.85f, 0.4f, 1.0f));

	if (Text_InteractPrompt) Text_InteractPrompt->SetVisibility(ESlateVisibility::Hidden);
}

void UHUDWidget::NativeDestruct()
{
	UnbindFromCharacter();
	Super::NativeDestruct();
}

void UHUDWidget::BindToCharacter(ABaseCharacter* InCharacter)
{
	UnbindFromCharacter();

	if (!InCharacter) return;

	CachedCharacter = InCharacter;
	CachedCombat    = InCharacter->CombatComponent;
	CachedHealth    = InCharacter->HealthComponent;
	CachedStamina   = InCharacter->StaminaComponent;

	if (UCombatComponent* CC = CachedCombat.Get())
	{
		CC->OnAmmoChanged.AddDynamic(this, &UHUDWidget::HandleAmmoChanged);
		CC->OnWeaponChanged.AddDynamic(this, &UHUDWidget::HandleWeaponChanged);
		// Push initial values (delegates only fire on change)
		HandleAmmoChanged(CC->GetCurrentAmmo(), CC->GetMaxAmmo());
		HandleWeaponChanged(CC->GetCurrentWeaponName());
	}
	if (UHealthComponent* HC = CachedHealth.Get())
	{
		HC->OnHealthChanged.AddDynamic(this, &UHUDWidget::HandleHealthChanged);
		HandleHealthChanged(HC->CurrentHealth, HC->MaxHealth);
	}
	if (UStaminaComponent* SC = CachedStamina.Get())
	{
		SC->OnStaminaChanged.AddDynamic(this, &UHUDWidget::HandleStaminaChanged);
		HandleStaminaChanged(SC->CurrentStamina, SC->MaxStamina);
	}
}

void UHUDWidget::UnbindFromCharacter()
{
	if (UCombatComponent* CC = CachedCombat.Get())
	{
		CC->OnAmmoChanged.RemoveDynamic(this, &UHUDWidget::HandleAmmoChanged);
		CC->OnWeaponChanged.RemoveDynamic(this, &UHUDWidget::HandleWeaponChanged);
	}
	if (UHealthComponent* HC = CachedHealth.Get())
	{
		HC->OnHealthChanged.RemoveDynamic(this, &UHUDWidget::HandleHealthChanged);
	}
	if (UStaminaComponent* SC = CachedStamina.Get())
	{
		SC->OnStaminaChanged.RemoveDynamic(this, &UHUDWidget::HandleStaminaChanged);
	}
	CachedCombat = nullptr;
	CachedHealth = nullptr;
	CachedStamina = nullptr;
	CachedCharacter = nullptr;
}

// --- Handlers receive VALUES only — no component lookups ---

void UHUDWidget::HandleAmmoChanged(int32 CurrentAmmo, int32 MaxAmmo)
{
	if (Text_CurrentAmmo) Text_CurrentAmmo->SetText(FText::AsNumber(CurrentAmmo));
	if (Text_MaxAmmo)     Text_MaxAmmo->SetText(FText::AsNumber(MaxAmmo));
	if (StatBar_Ammo)     StatBar_Ammo->SetStatValues((float)CurrentAmmo, (float)MaxAmmo);
}

void UHUDWidget::HandleWeaponChanged(const FString& WeaponName)
{
	if (Text_WeaponName) Text_WeaponName->SetText(FText::FromString(WeaponName));
}

void UHUDWidget::HandleHealthChanged(float Current, float Max)
{
	if (StatBar_Health) StatBar_Health->SetStatValues(Current, Max);
}

void UHUDWidget::HandleStaminaChanged(float Current, float Max)
{
	if (StatBar_Stamina) StatBar_Stamina->SetStatValues(Current, Max);
}

// Interact prompt still needs a per-frame trace — that's the only polling left
void UHUDWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	ABaseCharacter* Char = CachedCharacter.Get();
	if (!Char || !Text_InteractPrompt) return;

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