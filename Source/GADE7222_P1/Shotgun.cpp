#include "Shotgun.h"
#include "WeaponDataAsset.h"
#include "ProjectileBase.h"
#include "BaseCharacter.h"
#include "Camera/CameraComponent.h"
#include "Engine/World.h"

void AShotgun::Fire()
{
	if (!WeaponData || !GetCharacterOwner()) return;

	// Check ammo pool
	if (!WeaponData->bInfiniteAmmo)
	{
		if (CurrentAmmo <= 0)
		{
			StopFire();
			return;
		}
		CurrentAmmo--;
	}

	ABaseCharacter* OwnerChar = GetCharacterOwner();
	if (!OwnerChar || !OwnerChar->PlayerCamera || !WeaponData->ProjectileClass) return;

	FVector Start = OwnerChar->PlayerCamera->GetComponentLocation();
	FRotator BaseRotation = OwnerChar->PlayerCamera->GetComponentRotation();

	// Spawn multiple projectiles for the shotgun spread
	for (int32 i = 0; i < WeaponData->ProjectilesPerShot; i++)
	{
		// Apply random spread
		FRotator SpreadRot = BaseRotation;
		SpreadRot.Pitch += FMath::RandRange(-WeaponData->SpreadAngle, WeaponData->SpreadAngle);
		SpreadRot.Yaw += FMath::RandRange(-WeaponData->SpreadAngle, WeaponData->SpreadAngle);

		FActorSpawnParameters SpawnParams;
		SpawnParams.Owner = OwnerChar;
		SpawnParams.Instigator = OwnerChar;

		GetWorld()->SpawnActor<AProjectileBase>(WeaponData->ProjectileClass, Start, SpreadRot, SpawnParams);
	}
}