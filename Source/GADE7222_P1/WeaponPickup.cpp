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