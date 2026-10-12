#include "Pistol.h"
#include "WeaponDataAsset.h"
#include "BaseCharacter.h"
#include "Camera/CameraComponent.h"
#include "Engine/World.h"
#include "DrawDebugHelpers.h"
#include "DamageableInterface.h"

void APistol::Fire()
{
	if (!WeaponData || !GetCharacterOwner()) return;

	if (!WeaponData->bInfiniteAmmo)
	{
		if (CurrentAmmo <= 0) { StopFire(); return; }
		CurrentAmmo--;
	}

	PerformTrace();
}

void APistol::PerformTrace()
{
	ABaseCharacter* OwnerChar = GetCharacterOwner();
	if (!OwnerChar || !OwnerChar->PlayerCamera) return;

	//Trace from CAMERA (player's eyes) for accurate aiming
	FVector CamStart = OwnerChar->PlayerCamera->GetComponentLocation();
	FVector CamEnd = CamStart + OwnerChar->PlayerCamera->GetForwardVector() * 10000.0f;

	FHitResult Hit;
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(this);
	Params.AddIgnoredActor(OwnerChar);
	Params.bTraceComplex = true;

	FVector MuzzleLoc = GetMuzzleLocation(); // bullets visibly leave the barrel
	FVector ImpactPoint = CamEnd;

	if (GetWorld()->LineTraceSingleByChannel(Hit, CamStart, CamEnd, ECC_Visibility, Params))
	{
		ImpactPoint = Hit.ImpactPoint;
		// --- Interface-based damage (no cast to ABaseCharacter!) ---
		AActor* HitActor = Hit.GetActor();
		if (HitActor && HitActor != OwnerChar &&
			HitActor->GetClass()->ImplementsInterface(UDamageableInterface::StaticClass()))
		{
			IDamageableInterface::Execute_ReceiveDamage(
				HitActor, WeaponData->Damage, OwnerChar, ImpactPoint);
		}
	}

	// Draw a beam from MUZZLE → IMPACT so it looks like the bullet came from the gun
	//Must replace this with a Niagara/particle tracer p2
	DrawDebugLine(GetWorld(), MuzzleLoc, ImpactPoint, FColor::Yellow, false, 0.05f, 0, 1.5f);
}