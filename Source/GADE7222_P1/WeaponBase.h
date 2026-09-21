#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "WeaponBase.generated.h"

class UWeaponDataAsset;

UCLASS(Abstract)
class GADE7222_P1_API AWeaponBase : public AActor
{
	GENERATED_BODY()

public:
	AWeaponBase();

	virtual void StartFire();
	virtual void StopFire();

	// Pure virtual: Children MUST implement their own firing logic
	virtual void Fire() PURE_VIRTUAL(AWeaponBase::Fire, );

	// Ammo management
	UFUNCTION(BlueprintCallable, Category = "Weapon")
	virtual void AddAmmo(int32 Amount);

	// Getters
	FORCEINLINE int32 GetCurrentAmmo() const { return CurrentAmmo; }
	FORCEINLINE UWeaponDataAsset* GetWeaponData() const { return WeaponData; }
	FORCEINLINE bool IsFiring() const { return bIsFiring; }

protected:
	virtual void BeginPlay() override;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
	UWeaponDataAsset* WeaponData;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	USkeletalMeshComponent* WeaponMesh;

	FTimerHandle FireTimerHandle;

	int32 CurrentAmmo;
	bool bIsFiring = false;

	// Helper to get the owner character
	class ABaseCharacter* GetCharacterOwner() const;
};