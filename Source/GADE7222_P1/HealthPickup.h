#pragma once

#include "CoreMinimal.h"
#include "PickupBase.h"
#include "HealthPickup.generated.h"

UCLASS()
class GADE7222_P1_API AHealthPickup : public APickupBase
{
	GENERATED_BODY()

public:
	AHealthPickup();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Health")
	float HealAmount = 25.0f;

	// Only pick up if the character is not at full HP
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Health")
	bool bBlockAtFullHealth = true;

protected:
	virtual bool OnPickedUp_Implementation(AActor* Interactor) override;
	virtual bool CanBePickedUp_Implementation(AActor* Interactor) const override;
};