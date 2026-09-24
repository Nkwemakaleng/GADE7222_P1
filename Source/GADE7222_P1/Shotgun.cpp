#include "Shotgun.h"
#include "WeaponDataAsset.h"
#include "ProjectileBase.h"
#include "BaseCharacter.h"
#include "Engine/World.h"

void AShotgun::Fire()
{
	if (!WeaponData || !GetCharacterOwner())
	{
		UE_LOG(LogTemp, Error, TEXT("Shotgun::Fire - WeaponData or Owner is NULL"));
		return;
	}

	ABaseCharacter* OwnerChar = GetCharacterOwner();

	// Ammo check
	if (!WeaponData->bInfiniteAmmo)
	{
		if (CurrentAmmo <= 0) { StopFire(); return; }
		CurrentAmmo--;
	}

	// --- DIAGNOSTIC LOGS ---
	UE_LOG(LogTemp, Warning, TEXT("Shotgun::Fire - ProjectilesPerShot=%d, SpreadAngle=%f"),
		WeaponData->ProjectilesPerShot, WeaponData->SpreadAngle);
	UE_LOG(LogTemp, Warning, TEXT("Shotgun::Fire - ProjectileClass=%s"),
		WeaponData->ProjectileClass ? *WeaponData->ProjectileClass->GetName() : TEXT("NULL!"));

	if (!WeaponData->ProjectileClass)
	{
		UE_LOG(LogTemp, Error, TEXT("Shotgun::Fire - ProjectileClass is NULL! Assign BP_Projectile in DA_AttacheShotgun."));
		return;
	}

	if (!OwnerChar || !OwnerChar->PlayerCamera)
	{
		UE_LOG(LogTemp, Error, TEXT("Shotgun::Fire - Owner has no Camera"));
		return;
	}

	FVector MuzzleLoc = GetMuzzleLocation();
	FVector AimPoint = GetAimPoint();
	FRotator BaseRotation = (AimPoint - MuzzleLoc).Rotation();

	UE_LOG(LogTemp, Warning, TEXT("Shotgun::Fire - MuzzleLoc=%s, AimPoint=%s, Spawning %d projectiles"),
		*MuzzleLoc.ToString(), *AimPoint.ToString(), WeaponData->ProjectilesPerShot);

	for (int32 i = 0; i < WeaponData->ProjectilesPerShot; i++)
	{
		FRotator SpreadRot = BaseRotation;
		SpreadRot.Pitch += FMath::RandRange(-WeaponData->SpreadAngle, WeaponData->SpreadAngle);
		SpreadRot.Yaw   += FMath::RandRange(-WeaponData->SpreadAngle, WeaponData->SpreadAngle);

		FActorSpawnParameters SpawnParams;
		SpawnParams.Owner = OwnerChar;
		SpawnParams.Instigator = OwnerChar;
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

		AProjectileBase* Projectile = GetWorld()->SpawnActor<AProjectileBase>(
			WeaponData->ProjectileClass, MuzzleLoc, SpreadRot, SpawnParams);

		if (!Projectile)
		{
			UE_LOG(LogTemp, Error, TEXT("Shotgun::Fire - SpawnActor FAILED for projectile %d"), i);
		}
	}
}