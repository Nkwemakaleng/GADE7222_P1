#include "WeaponBase.h"
#include "WeaponDataAsset.h"
#include "Components/SkeletalMeshComponent.h"
#include "BaseCharacter.h"
#include "TimerManager.h"

AWeaponBase::AWeaponBase()
{
	PrimaryActorTick.bCanEverTick = false;

	WeaponMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("WeaponMesh"));
	RootComponent = WeaponMesh;
}

void AWeaponBase::BeginPlay()
{
	Super::BeginPlay();

	if (WeaponData)
	{
		CurrentAmmo = WeaponData->MaxAmmoPool;
	}
}

void AWeaponBase::StartFire()
{
	if (bIsFiring || !WeaponData) return;

	bIsFiring = true;
	
	// Fire immediately, then start the timer for automatic fire
	Fire();

	if (WeaponData->FireRate > 0.0f)
	{
		GetWorldTimerManager().SetTimer(FireTimerHandle, this, &AWeaponBase::Fire, WeaponData->FireRate, true);
	}
}

void AWeaponBase::StopFire()
{
	bIsFiring = false;
	GetWorldTimerManager().ClearTimer(FireTimerHandle);
}

void AWeaponBase::AddAmmo(int32 Amount)
{
	if (!WeaponData) return;
	
	// Clamp ammo so it never exceeds the max pool
	CurrentAmmo = FMath::Clamp(CurrentAmmo + Amount, 0, WeaponData->MaxAmmoPool);
}

ABaseCharacter* AWeaponBase::GetCharacterOwner() const
{
	return Cast<ABaseCharacter>(GetOwner());
}