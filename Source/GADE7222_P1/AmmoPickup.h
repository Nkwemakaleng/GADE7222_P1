#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "AmmoPickup.generated.h"

UCLASS()
class GADE7222_P1_API AAmmoPickup : public AActor
{
	GENERATED_BODY()
	
public:	
	AAmmoPickup();

protected:
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, Category = "Components")
	class USphereComponent* PickupSphere;

	UPROPERTY(VisibleAnywhere, Category = "Components")
	class UStaticMeshComponent* PickupMesh;

	// What weapon does this ammo belong to?
	UPROPERTY(EditAnywhere, Category = "Pickup")
	TSubclassOf<class AWeaponBase> WeaponToPickup;

	// How much ammo is added to the pool?
	UPROPERTY(EditAnywhere, Category = "Pickup")
	int32 AmmoAmount = 10;

	UFUNCTION()
	void OnOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult);
};