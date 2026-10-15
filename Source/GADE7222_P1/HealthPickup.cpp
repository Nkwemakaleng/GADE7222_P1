#include "HealthPickup.h"
#include "BaseCharacter.h"
#include "HealthComponent.h"

AHealthPickup::AHealthPickup()
{
	InteractPrompt = FText::FromString(TEXT("Pick up Health"));
	bAutoPickupOnOverlap = true;
	bDestroyOnPickup = true;
	Category = EPickupCategory::Health;
}

bool AHealthPickup::OnPickedUp_Implementation(AActor* Interactor)
{
	ABaseCharacter* Char = Cast<ABaseCharacter>(Interactor);
	if (!Char || !Char->HealthComponent) return false;

	Char->HealthComponent->Heal(HealAmount);
	return true;
}

bool AHealthPickup::CanBePickedUp_Implementation(AActor* Interactor) const
{
	if (!bBlockAtFullHealth) return true;

	ABaseCharacter* Char = Cast<ABaseCharacter>(Interactor);
	if (!Char || !Char->HealthComponent) return false;

	// Block if already at full health
	return Char->HealthComponent->CurrentHealth < Char->HealthComponent->MaxHealth;
}