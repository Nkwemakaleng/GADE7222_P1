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