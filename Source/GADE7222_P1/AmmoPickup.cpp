#include "AmmoPickup.h"
#include "BaseCharacter.h"
#include "CombatComponent.h"
#include "WeaponBase.h"

AAmmoPickup::AAmmoPickup()
{
	InteractPrompt = FText::FromString(TEXT("Pick up Ammo"));
	bAutoPickupOnOverlap = true;
	bDestroyOnPickup = true;
	Category= EPickupCategory::Ammo;
}
//Handles ammo adding and Weapon adding
bool AAmmoPickup::OnPickedUp_Implementation(AActor* Interactor)
{
	
	if (!WeaponToPickup) return false;

	ABaseCharacter* Char = Cast<ABaseCharacter>(Interactor);
	if (!Char || !Char->CombatComponent) return false;

	Char->PickupWeapon(WeaponToPickup, AmmoAmount);
	return true;
}
bool AAmmoPickup::CanBePickedUp_Implementation(AActor* Interactor) const
{
	if (!WeaponToPickup) return false;

	ABaseCharacter* Char = Cast<ABaseCharacter>(Interactor);
	if (!Char || !Char->CombatComponent) return false;

	// If the bot already has max ammo for this gun, return false so it stops trying
	return Char->CombatComponent->WantsWeaponOrAmmoPickup(WeaponToPickup);
} 
