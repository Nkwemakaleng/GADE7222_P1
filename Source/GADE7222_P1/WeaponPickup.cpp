#include "WeaponPickup.h"
#include "BaseCharacter.h"
#include "CombatComponent.h"
#include "WeaponBase.h"

AWeaponPickup::AWeaponPickup()
{
	InteractPrompt = FText::FromString(TEXT("Pick up Weapon"));
	bAutoPickupOnOverlap = false;    // Require E press
	bDestroyOnPickup = true;
	Category = EPickupCategory::Weapon;
}

bool AWeaponPickup::OnPickedUp_Implementation(AActor* Interactor)
{
	if (!WeaponClass) return false;

	ABaseCharacter* Char = Cast<ABaseCharacter>(Interactor);
	if (!Char || !Char->CombatComponent) return false;

	Char->PickupWeapon(WeaponClass, StartingAmmo);
	return true;
}
bool AWeaponPickup::CanBePickedUp_Implementation(AActor* Interactor) const
{
	if (!WeaponClass) return false;

	ABaseCharacter* Char = Cast<ABaseCharacter>(Interactor);
	if (!Char || !Char->CombatComponent) return false;

	// If the bot already has max ammo for this gun, return false so it stops trying
	return Char->CombatComponent->WantsWeaponOrAmmoPickup(WeaponClass);
}