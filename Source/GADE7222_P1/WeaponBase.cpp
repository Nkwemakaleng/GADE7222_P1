#include "WeaponBase.h"
#include "WeaponDataAsset.h"
#include "Components/SkeletalMeshComponent.h"
#include "BaseCharacter.h"
#include "TimerManager.h"
#include "Camera/CameraComponent.h"
#include "Engine/World.h"

AWeaponBase::AWeaponBase()
{
	PrimaryActorTick.bCanEverTick = false;

	WeaponMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("WeaponMesh"));
	RootComponent = WeaponMesh;
	
	// no collison on weapon mesh just visual
	WeaponMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	WeaponMesh->SetCollisionProfileName(TEXT("NoCollision"));
	WeaponMesh->SetGenerateOverlapEvents(false);
	WeaponMesh->SetSimulatePhysics(false);
	WeaponMesh->SetCanEverAffectNavigation(false);
}

void AWeaponBase::BeginPlay()
{
	Super::BeginPlay();

	if (WeaponData)
	{
		CurrentAmmo = WeaponData->MaxAmmoPool;
	}

	// --- Find the MuzzlePoint scene component inside the weapon Blueprint ---
	TArray<USceneComponent*> AllComponents;
	GetComponents<USceneComponent>(AllComponents);

	for (USceneComponent* Comp : AllComponents)
	{
		if (Comp && Comp->GetFName() == TEXT("Muzzle"))
		{
			MuzzlePointComponent = Comp;
			UE_LOG(LogTemp, Warning, TEXT("%s: Found MuzzlePoint component."), *GetName());
			break;
		}
	}

	if (!MuzzlePointComponent)
	{
		UE_LOG(LogTemp, Warning, TEXT("%s: No MuzzlePoint component found. Will fall back to socket / actor origin."), *GetName());
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

FVector AWeaponBase::GetMuzzleLocation() const
{
	// 1. Prefer the scene component
	if (MuzzlePointComponent)
	{
		return MuzzlePointComponent->GetComponentLocation();
	}

	// 2. Fall back to socket
	if (WeaponMesh && WeaponMesh->DoesSocketExist(MuzzleSocketName))
	{
		return WeaponMesh->GetSocketLocation(MuzzleSocketName);
	}

	// 3. Last resort
	return GetActorLocation();
}

FVector AWeaponBase::GetAimPoint(float MaxRange) const
{
	ABaseCharacter* OwnerChar = GetCharacterOwner();
	if (!OwnerChar || !OwnerChar->PlayerCamera)
	{
		return GetMuzzleLocation() + GetActorForwardVector() * MaxRange;
	}

	// Camera is the player's eyes — trace from there to find the crosshair target
	FVector CamStart = OwnerChar->PlayerCamera->GetComponentLocation();
	FVector CamEnd = CamStart + OwnerChar->PlayerCamera->GetForwardVector() * MaxRange;

	FHitResult Hit;
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(this);
	Params.AddIgnoredActor(OwnerChar);
	Params.bTraceComplex = true;

	if (GetWorld()->LineTraceSingleByChannel(Hit, CamStart, CamEnd, ECC_Visibility, Params))
	{
		return Hit.ImpactPoint;
	}
	return CamEnd; // No hit — aim far away
}

FRotator AWeaponBase::GetAimRotationFromMuzzle(const FVector& AimPoint) const
{
	FVector MuzzleLoc = GetMuzzleLocation();
	return (AimPoint - MuzzleLoc).Rotation();
}

ABaseCharacter* AWeaponBase::GetCharacterOwner() const
{
	return Cast<ABaseCharacter>(GetOwner());
}