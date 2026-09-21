#include "CombatComponent.h"
#include "BaseCharacter.h"
#include "WeaponBase.h"
#include "WeaponDataAsset.h"
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
		InitializeWeapons();
	}
}

void UCombatComponent::InitializeWeapons()
{
	// Spawn default weapons here (e.g., the starting Sword/Melee weapon)
	// Example: 
	// FActorSpawnParameters Params; Params.Owner = OwnerCharacter;
	// AWeaponBase* StartWeapon = GetWorld()->SpawnActor<AWeaponBase>(StartWeaponClass, Params);
	// WeaponInventory.Add(StartWeapon);
	// SwitchWeapon(0);
}

void UCombatComponent::HandleWeaponPickup(TSubclassOf<AWeaponBase> WeaponClass, int32 AmmoAmount)
{
	if (!OwnerCharacter || !WeaponClass) return;

	// 1. Check if we ALREADY have this weapon in our inventory
	for (AWeaponBase* Weapon : WeaponInventory)
	{
		if (Weapon && Weapon->GetClass() == WeaponClass)
		{
			// We already have it! Add ammo directly to the ammo pool.
			Weapon->AddAmmo(AmmoAmount);
			OnAmmoChanged.Broadcast(GetCurrentAmmo(), GetMaxAmmo());
			UE_LOG(LogTemp, Warning, TEXT("Weapon already owned. Added %d ammo to pool."), AmmoAmount);
			return; // Exit early
		}
	}

	// 2. We DON'T have it. Spawn the weapon and add to inventory.
	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = OwnerCharacter;
	SpawnParams.Instigator = OwnerCharacter;

	AWeaponBase* NewWeapon = GetWorld()->SpawnActor<AWeaponBase>(WeaponClass, SpawnParams);
	if (NewWeapon)
	{
		WeaponInventory.Add(NewWeapon);
		
		// Give the initial ammo amount from the pickup
		NewWeapon->AddAmmo(AmmoAmount);
		
		// Auto-equip the new weapon
		CurrentWeaponIndex = WeaponInventory.Num() - 1;
		SwitchWeapon(CurrentWeaponIndex);
		OnWeaponChanged.Broadcast(GetCurrentWeaponName());
		OnAmmoChanged.Broadcast(GetCurrentAmmo(), GetMaxAmmo());
		UE_LOG(LogTemp, Warning, TEXT("Picked up new weapon!"));
	}
}

void UCombatComponent::StartFire()
{
	if (CurrentWeapon) CurrentWeapon->StartFire();
}

void UCombatComponent::StopFire()
{
	if (CurrentWeapon) CurrentWeapon->StopFire();
}

void UCombatComponent::SwitchWeapon(int32 WeaponIndex)
{
	if (WeaponInventory.IsValidIndex(WeaponIndex))
	{
		if (CurrentWeapon) CurrentWeapon->StopFire(); // Stop firing old weapon
		
		CurrentWeaponIndex = WeaponIndex;
		CurrentWeapon = WeaponInventory[CurrentWeaponIndex];
		
		// Attach the weapon mesh to the character's hand socket
		if (OwnerCharacter && OwnerCharacter->GetMesh())
		{
			CurrentWeapon->AttachToComponent(OwnerCharacter->GetMesh(), FAttachmentTransformRules::SnapToTargetIncludingScale, TEXT("HandGripSocket"));
		}
		
		UE_LOG(LogTemp, Warning, TEXT("Switched to weapon: %s"), *CurrentWeapon->GetName());
		// Notify the HUD
		OnWeaponChanged.Broadcast(GetCurrentWeaponName());
		OnAmmoChanged.Broadcast(GetCurrentAmmo(), GetMaxAmmo());
	}
}

void UCombatComponent::NextWeapon()
{
	if (WeaponInventory.Num() == 0) return;
	int32 NextIndex = (CurrentWeaponIndex + 1) % WeaponInventory.Num();
	SwitchWeapon(NextIndex);
}
int32 UCombatComponent::GetCurrentAmmo() const
{
	return CurrentWeapon ? CurrentWeapon->GetCurrentAmmo() : 0;
}

int32 UCombatComponent::GetMaxAmmo() const
{
	if (CurrentWeapon && CurrentWeapon->GetWeaponData())
	{
		return CurrentWeapon->GetWeaponData()->MaxAmmoPool;
	}
	return 0;
}

FString UCombatComponent::GetCurrentWeaponName() const
{
	if (CurrentWeapon && CurrentWeapon->GetWeaponData())
	{
		return CurrentWeapon->GetWeaponData()->WeaponName;
	}
	return TEXT("None");
}