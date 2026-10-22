#include "CombatComponent.h"
#include "BaseCharacter.h"
#include "WeaponBase.h"
#include "WeaponDataAsset.h"
#include "GameFramework/Character.h"
#include "Components/SkeletalMeshComponent.h"
#include "Engine/World.h"

UCombatComponent::UCombatComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UCombatComponent::BeginPlay()
{
	Super::BeginPlay();
	OwnerCharacter = Cast<ABaseCharacter>(GetOwner());

	if (OwnerCharacter)
	{
		CacheAttachPoint();
	}
}

void UCombatComponent::CacheAttachPoint()
{
	CachedAttachPoint = nullptr;

	if (!OwnerCharacter) return;

	// Search all scene components on the character for one named "WeaponAttachPoint"
	TArray<USceneComponent*> AllComponents;
	OwnerCharacter->GetComponents<USceneComponent>(AllComponents);

	for (USceneComponent* Comp : AllComponents)
	{
		if (Comp && Comp->GetFName() == TEXT("WeaponAttachPoint"))
		{
			CachedAttachPoint = Comp;
			UE_LOG(LogTemp, Warning, TEXT("CombatComponent: Found WeaponAttachPoint on %s"), *OwnerCharacter->GetName());
			break;
		}
	}

	if (!CachedAttachPoint)
	{
		UE_LOG(LogTemp, Error, TEXT("CombatComponent: NO WeaponAttachPoint found on %s! Weapons will not attach."), *OwnerCharacter->GetName());
	}
}

void UCombatComponent::InitializeWithWeapons(const TArray<TSubclassOf<AWeaponBase>>& Weapons)
{
	if (!OwnerCharacter) return;

	// Clean up any pre-existing weapons
	for (AWeaponBase* W : WeaponInventory)
	{
		if (W) W->Destroy();
	}
	WeaponInventory.Empty();
	CurrentWeapon = nullptr;
	CurrentWeaponIndex = -1;   // important: -1 so the first SwitchWeapon actually switches

	for (TSubclassOf<AWeaponBase> WeaponClass : Weapons)
	{
		if (!WeaponClass) continue;

		FActorSpawnParameters SpawnParams;
		SpawnParams.Owner = OwnerCharacter;
		SpawnParams.Instigator = OwnerCharacter;
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

		AWeaponBase* NewWeapon = GetWorld()->SpawnActor<AWeaponBase>(WeaponClass, SpawnParams);
		if (NewWeapon)
		{
			// Hide until explicitly equipped
			NewWeapon->SetActorHiddenInGame(true);
			NewWeapon->SetActorEnableCollision(false);
			NewWeapon->SetActorTickEnabled(false);
			WeaponInventory.Add(NewWeapon);
			
		}
	}

	if (WeaponInventory.Num() > 0)
	{
		SwitchWeapon(0);
	}
}

void UCombatComponent::HandleWeaponPickup(TSubclassOf<AWeaponBase> WeaponClass, int32 AmmoAmount)
{
	if (!OwnerCharacter || !WeaponClass) return;

	// Already have it? Add ammo.
	for (AWeaponBase* Weapon : WeaponInventory)
	{
		if (Weapon && Weapon->GetClass() == WeaponClass)
		{
			Weapon->AddAmmo(AmmoAmount);
			OnAmmoChanged.Broadcast(GetCurrentAmmo(), GetMaxAmmo());
			UE_LOG(LogTemp, Warning, TEXT("Added %d ammo to existing weapon."), AmmoAmount);
			return;
		}
	}

	// New weapon
	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = OwnerCharacter;
	SpawnParams.Instigator = OwnerCharacter;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	AWeaponBase* NewWeapon = GetWorld()->SpawnActor<AWeaponBase>(WeaponClass, SpawnParams);
    	if (NewWeapon)
    	{
    		WeaponInventory.Add(NewWeapon);
    		NewWeapon->AddAmmo(AmmoAmount);
    		
    		// Auto-equip the new weapon (Pass the index directly)
    		SwitchWeapon(WeaponInventory.Num() - 1);
    		
    		UE_LOG(LogTemp, Warning, TEXT("Picked up new weapon!"));
    	}
}

void UCombatComponent::SwitchWeapon(int32 WeaponIndex)
{
	if (!WeaponInventory.IsValidIndex(WeaponIndex)) return;
	if (WeaponIndex == CurrentWeaponIndex && CurrentWeapon) return;

	// Put the OLD weapon to sleep 
	if (CurrentWeapon)
	{
		CurrentWeapon->StopFire();
		CurrentWeapon->SetActorHiddenInGame(true);
		CurrentWeapon->SetActorEnableCollision(false);
		CurrentWeapon->SetActorTickEnabled(false);
		CurrentWeapon->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
	}

	//  Equip the NEW weapon 
	CurrentWeaponIndex = WeaponIndex;
	CurrentWeapon = WeaponInventory[CurrentWeaponIndex];

	if (!CurrentWeapon) return;

	CurrentWeapon->SetActorHiddenInGame(false);
	CurrentWeapon->SetActorEnableCollision(false);   // mesh still no collision
	CurrentWeapon->SetActorTickEnabled(true);

	// Attach to the character's WeaponAttachPoint 
	if (!CachedAttachPoint) CacheAttachPoint();      // in case it wasn't found yet

	if (CachedAttachPoint)
	{
		CurrentWeapon->AttachToComponent(
			CachedAttachPoint,
			FAttachmentTransformRules::SnapToTargetIncludingScale
		);
		UE_LOG(LogTemp, Warning, TEXT("Equipped %s → attached to WeaponAttachPoint"), *CurrentWeapon->GetName());
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("SwitchWeapon: No WeaponAttachPoint — weapon stays at world origin."));
	}

	// Notify HUD 
	OnWeaponChanged.Broadcast(GetCurrentWeaponName());
	OnAmmoChanged.Broadcast(GetCurrentAmmo(), GetMaxAmmo());
}

void UCombatComponent::NextWeapon()
{
	if (WeaponInventory.Num() == 0) return;
	int32 NextIndex = (CurrentWeaponIndex + 1) % WeaponInventory.Num();
	SwitchWeapon(NextIndex);
}

void UCombatComponent::StartFire()
{
	if (!CurrentWeapon)
	{
		UE_LOG(LogTemp, Error, TEXT("StartFire: CurrentWeapon is NULL"));
		return;
	}

	UE_LOG(LogTemp, Warning, TEXT("StartFire on: %s (Ammo=%d, Hidden=%d, Class=%s)"),
		*CurrentWeapon->GetName(),
		CurrentWeapon->GetCurrentAmmo(),
		CurrentWeapon->IsHidden(),
		*CurrentWeapon->GetClass()->GetName());

	CurrentWeapon->StartFire();
	OnAmmoChanged.Broadcast(GetCurrentAmmo(), GetMaxAmmo());
}

void UCombatComponent::StopFire()
{
	if (CurrentWeapon) CurrentWeapon->StopFire();
}

int32 UCombatComponent::GetCurrentAmmo() const
{
	return CurrentWeapon ? CurrentWeapon->GetCurrentAmmo() : 0;
}

int32 UCombatComponent::GetMaxAmmo() const
{
	if (CurrentWeapon && CurrentWeapon->GetWeaponData())
		return CurrentWeapon->GetWeaponData()->MaxAmmoPool;
	return 0;
}

FString UCombatComponent::GetCurrentWeaponName() const
{
	if (CurrentWeapon && CurrentWeapon->GetWeaponData())
		return CurrentWeapon->GetWeaponData()->WeaponName;
	return TEXT("None");
}

void UCombatComponent::DestroyAllWeapons()
{
	// Stop firing anything first
	if (CurrentWeapon)
	{
		CurrentWeapon->StopFire();
		CurrentWeapon = nullptr;
	}

	for (AWeaponBase* W : WeaponInventory)
	{
		if (IsValid(W))
		{
			W->Destroy();
		}
	}

	WeaponInventory.Empty();
	CurrentWeaponIndex = -1;

	UE_LOG(LogTemp, Warning, TEXT("CombatComponent: destroyed all weapons."));
}

void UCombatComponent::SetWeaponsVisible(bool bVisible)
{
	for (AWeaponBase* W : WeaponInventory)
	{
		if (IsValid(W))
		{
			W->SetActorHiddenInGame(!bVisible);
		}
	}
}

bool UCombatComponent::HasWeaponInSlot(int32 SlotIndex) const
{
	return WeaponInventory.IsValidIndex(SlotIndex);
}


AWeaponBase* UCombatComponent::GetWeaponInSlot(int32 SlotIndex) const
{
	return WeaponInventory.IsValidIndex(SlotIndex) ? WeaponInventory[SlotIndex] : nullptr;
}

int32 UCombatComponent::FindBestCombatWeaponIndex(float DistanceToTarget) const
{
	int32 BestIndex = INDEX_NONE;
	int32 BestPriority = MIN_int32;

	for (int32 i = 0; i < WeaponInventory.Num(); i++)
	{
		AWeaponBase* W = WeaponInventory[i];
		if (!W) continue;

		const UWeaponDataAsset* Data = W->GetWeaponData();
		if (!Data) continue;

		const bool bHasAmmo = Data->bInfiniteAmmo || W->GetCurrentAmmo() > 0;
		if (!bHasAmmo) continue;

		// --- Range-Based Filtering ---
		// If we have a distance, and this is a Shotgun/Projectile, require closer range
		if (DistanceToTarget > 0.0f && Data->WeaponType == EWeaponType::Projectile)
		{
			if (DistanceToTarget > 800.0f) 
			{
				continue; // Skip the shotgun, we are too far away
			}
		}

		if (Data->CombatPriority > BestPriority)
		{
			BestPriority = Data->CombatPriority;
			BestIndex = i;
		}
	}

	return BestIndex;
}

bool UCombatComponent::SwitchToBestCombatWeapon(float DistanceToTarget)
{
	const int32 BestIndex = FindBestCombatWeaponIndex(DistanceToTarget);

	if (BestIndex == INDEX_NONE) return false;
	if (BestIndex == CurrentWeaponIndex) return false;

	SwitchWeapon(BestIndex);
	return true;
}

bool UCombatComponent::WantsWeaponOrAmmoPickup(TSubclassOf<AWeaponBase> PickupClass) const
{
	if (!PickupClass) return false;

	// 1. Check if we already own this specific weapon class
	for (AWeaponBase* W : WeaponInventory)
	{
		if (W && W->GetClass() == PickupClass)
		{
			const UWeaponDataAsset* Data = W->GetWeaponData();
			if (!Data) return false;

			// If we have infinite ammo, we NEVER want this pickup again
			if (Data->bInfiniteAmmo) return false;

			// We own it. Only pick it up if our ammo is NOT completely full.
			return W->GetCurrentAmmo() < Data->MaxAmmoPool;
		}
	}

	// 2. We don't own it. Do we have a free slot?
	return WeaponInventory.Num() < MaxWeaponSlots;
}