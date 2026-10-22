#pragma once

#include "CoreMinimal.h"
#include "PickupBase.h"
#include "AmmoPickup.generated.h"

class AWeaponBase;

UCLASS()
class GADE7222_P1_API AAmmoPickup : public APickupBase
{
	GENERATED_BODY()

public:
	AAmmoPickup();

	// Which weapon this ammo belongs to
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ammo")
	TSubclassOf<AWeaponBase> WeaponToPickup;

	// How much ammo to give
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ammo")
	int32 AmmoAmount = 10;

protected:
	virtual bool OnPickedUp_Implementation(AActor* Interactor) override;
	
	virtual bool CanBePickedUp_Implementation(AActor* Interactor) const override;
};