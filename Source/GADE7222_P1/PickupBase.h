#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "InteractableInterface.h"
#include "PickupBase.generated.h"

class USphereComponent;
class UStaticMeshComponent;
class URotatingMovementComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnPickupConsumedSignature, APickupBase*, Pickup, AActor*, Interactor);

UENUM(BlueprintType)
enum class EPickupCategory : uint8
{
	Generic  UMETA(DisplayName = "Generic"),
	Health   UMETA(DisplayName = "Health"),
	Weapon   UMETA(DisplayName = "Weapon"),   // gives the gun AND its ammo
	Ammo     UMETA(DisplayName = "Ammo"),     // ammo-only refill
	Key      UMETA(DisplayName = "Key")
};

UCLASS(Abstract, Blueprintable)
class GADE7222_P1_API APickupBase : public AActor, public IInteractableInterface
{
	GENERATED_BODY()

public:
	APickupBase();

	// --- Config ---

	// If true, walking over the pickup triggers it automatically (no E press)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pickup")
	bool bAutoPickupOnOverlap = true;

	// If false, the pickup stays in the world after being used (e.g. reusable ammo box)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pickup")
	bool bDestroyOnPickup = true;

	// Text shown in the interact prompt
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pickup")
	FText InteractPrompt = FText::FromString(TEXT("Pick up"));

	// Optional: rotate the pickup while it's in the world
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pickup")
	bool bRotate = true;

	// Optional: bob up and down
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pickup")
	bool bBob = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pickup")
	float BobHeight = 10.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pickup")
	float BobSpeed = 2.0f;

	// Cooldown before the pickup can be reused (only relevant if bDestroyOnPickup = false)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pickup")
	float ReuseCooldown = 0.0f;

	// Fires the moment a pickup is successfully consumed by an actor
	UPROPERTY(BlueprintAssignable, Category = "Pickup")
	FOnPickupConsumedSignature OnPickupConsumed;

	// --- IInteractableInterface ---
	virtual void Interact_Implementation(AActor* Interactor) override;
	virtual FText GetInteractPrompt_Implementation() const override;
	virtual bool CanInteract_Implementation(AActor* Interactor) const override;
	
	// Category used by AI / quests to classify this pickup. Overridden by children.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pickup")
	EPickupCategory Category = EPickupCategory::Generic;
protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

	// --- CHILDREN OVERRIDE THIS ---
	// Called when the pickup is successfully consumed. Return true if the pickup
	// was actually consumed (so base can decide to destroy or start cooldown).
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Pickup")
	bool OnPickedUp(AActor* Interactor);
	virtual bool OnPickedUp_Implementation(AActor* Interactor);

	// Optional gate: return false to block the pickup (e.g. player is at full HP)
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Pickup")
	bool CanBePickedUp(AActor* Interactor) const;
	virtual bool CanBePickedUp_Implementation(AActor* Interactor) const;

	// --- COMPONENTS (children can style/replace) ---
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	USphereComponent* PickupSphere;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UStaticMeshComponent* PickupMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	URotatingMovementComponent* RotatingMovement;

	// Sphere overlap handler
	UFUNCTION()
	void OnSphereOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
		bool bFromSweep, const FHitResult& SweepResult);

	// Cooldown timer handle
	FTimerHandle CooldownHandle;
	bool bIsOnCooldown = false;

	// Used to trigger the actual pickup logic (overlap and interact both funnel here)
	bool TryConsume(AActor* Interactor);

	// Visual state helpers
	void SetPickupActive(bool bActive);

private:
	FVector InitialLocation;
	float BobTimer = 0.0f;
};