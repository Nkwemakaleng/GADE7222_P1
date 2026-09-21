#include "Pistol.h"
#include "WeaponDataAsset.h"
#include "BaseCharacter.h"
#include "Camera/CameraComponent.h"
#include "Engine/World.h"

void APistol::Fire()
{
	if (!WeaponData || !GetCharacterOwner()) return;

	// Check ammo (unless infinite)
	if (!WeaponData->bInfiniteAmmo)
	{
		if (CurrentAmmo <= 0)
		{
			StopFire();
			return;
		}
		CurrentAmmo--;
	}

	PerformTrace();
}

void APistol::PerformTrace()
{
	ABaseCharacter* OwnerChar = GetCharacterOwner();
	if (!OwnerChar || !OwnerChar->PlayerCamera) return;

	FVector Start = OwnerChar->PlayerCamera->GetComponentLocation();
	FVector End = Start + (OwnerChar->PlayerCamera->GetForwardVector() * 10000.0f);
	FHitResult HitResult;
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(this);
	Params.AddIgnoredActor(OwnerChar);

	if (GetWorld()->LineTraceSingleByChannel(HitResult, Start, End, ECC_Visibility, Params))
	{
		// Apply damage (3 dmg)
		// HitResult.GetActor()->TakeDamage(WeaponData->Damage, FDamageEvent(), OwnerChar->GetController(), this);
	}
}