#pragma once

#include "CoreMinimal.h"
#include "WeaponBase.h"
#include "HitscanWeapon.generated.h"

UCLASS()
class GADE7222_P1_API AHitscanWeapon : public AWeaponBase
{
	GENERATED_BODY()

public:
	virtual void Fire() override;

protected:
	void PerformTrace();
};