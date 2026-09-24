#pragma once

#include "CoreMinimal.h"
#include "PickupBase.h"
#include "WeaponPickup.generated.h"

class AWeaponBase;

UCLASS()
class GADE7222_P1_API AWeaponPickup : public APickupBase
{
	GENERATED_BODY()

public:
	AWeaponPickup();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
	TSubclassOf<AWeaponBase> WeaponClass;

	// Starting ammo for the weapon when picked up fresh
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
	int32 StartingAmmo = 15;

protected:
	virtual bool OnPickedUp_Implementation(AActor* Interactor) override;
};