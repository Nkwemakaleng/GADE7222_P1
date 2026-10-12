#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PickupSpawner.generated.h"

class APickupBase;
class UBillboardComponent;
class UArrowComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPickupSpawnedSignature, APickupBase*, SpawnedPickup);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnPickupPickedUpSignature, APickupBase*, Pickup, AActor*, Picker);
UCLASS()
class GADE7222_P1_API APickupSpawner : public AActor
{
	GENERATED_BODY()

public:
	APickupSpawner();

	// What pickup to spawn (BP_HealthPickup, BP_AmmoPickup, etc.)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pickup Spawner")
	TSubclassOf<APickupBase> PickupClass;

	// Height above this spawner's origin (in cm)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pickup Spawner")
	float SpawnHeight = 100.0f;

	// Random horizontal scatter (X/Y) — useful when MaxActivePickups > 1
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pickup Spawner")
	float RandomHorizontalOffset = 0.0f;

	// Spawn immediately on BeginPlay
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pickup Spawner")
	bool bSpawnOnBeginPlay = true;

	// Delay before respawn after a pickup is consumed. -1 = never respawn.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pickup Spawner")
	float RespawnDelay = 10.0f;

	// How many pickups this spawner keeps alive at once
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pickup Spawner", meta = (ClampMin = "1"))
	int32 MaxActivePickups = 1;

	// Emitted whenever a new pickup is spawned
	UPROPERTY(BlueprintAssignable, Category = "Pickup Spawner")
	FOnPickupSpawnedSignature OnPickupSpawned;

	// Emitted whenever a pickup belonging to this spawner is consumed
    UPROPERTY(BlueprintAssignable, Category = "Pickup Spawner")
    FOnPickupPickedUpSignature OnPickupPickedUp;
	
	// Manually spawn a pickup right now (respects MaxActivePickups)
	UFUNCTION(BlueprintCallable, Category = "Pickup Spawner")
	APickupBase* SpawnPickupNow();

	// Start the respawn timer (called automatically when a pickup is consumed)
	UFUNCTION(BlueprintCallable, Category = "Pickup Spawner")
	void ScheduleRespawn();
	
	
protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	virtual void OnConstruction(const FTransform& Transform) override;

	// Fires when one of our spawned pickups is destroyed
	UFUNCTION()
	void HandleSpawnedPickupDestroyed(AActor* DestroyedActor);
	
	UFUNCTION()
    void HandlePickupConsumed(APickupBase* Pickup, AActor* Interactor);
    
	// Editor-only visualization
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UBillboardComponent* EditorIcon;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UArrowComponent* HeightArrow;

private:
	UPROPERTY()
	TArray<APickupBase*> ActivePickups;

	FTimerHandle RespawnTimerHandle;
	bool bShuttingDown = false;
};