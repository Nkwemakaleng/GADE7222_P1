#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "CombatComponent.generated.h"

class AWeaponBase;
class ABaseCharacter;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnAmmoChangedSignature, int32, CurrentAmmo, int32, MaxAmmo);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnWeaponChangedSignature, const FString&, WeaponName);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class GADE7222_P1_API UCombatComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UCombatComponent();

	void StartFire();
	void StopFire();
	
	void SwitchWeapon(int32 WeaponIndex);
	void NextWeapon();

	// This handles both picking up a new gun AND adding ammo to an owned one
	void HandleWeaponPickup(TSubclassOf<AWeaponBase> WeaponClass, int32 AmmoAmount);
	
	
	UPROPERTY(BlueprintAssignable, Category = "Combat Events")
	FOnAmmoChangedSignature OnAmmoChanged;

	UPROPERTY(BlueprintAssignable, Category = "Combat Events")
	FOnWeaponChangedSignature OnWeaponChanged;

	// Public getters for the HUD to poll
	UFUNCTION(BlueprintPure, Category = "Combat")
	int32 GetCurrentAmmo() const;

	UFUNCTION(BlueprintPure, Category = "Combat")
	int32 GetMaxAmmo() const;

	UFUNCTION(BlueprintPure, Category = "Combat")
	FString GetCurrentWeaponName() const;

	UFUNCTION(BlueprintCallable, Category = "Combat")
	void InitializeWithWeapons(const TArray<TSubclassOf<class AWeaponBase>>& Weapons);

	UFUNCTION(BlueprintPure, Category = "Combat")
	bool HasWeaponInSlot(int32 SlotIndex) const;
	
	UFUNCTION(BlueprintCallable, Category = "Combat")
	void DestroyAllWeapons();

	UFUNCTION(BlueprintCallable, Category = "Combat")
	void SetWeaponsVisible(bool bVisible);
	
	// Returns the index of the best weapon to use right now (highest CombatPriority with ammo).
	// Returns INDEX_NONE if no viable weapon exists.
	UFUNCTION(BlueprintPure, Category = "Combat")
	int32 FindBestCombatWeaponIndex() const;

	// Switches to the best weapon if it's not already equipped.
	// Returns true if a switch happened.
	UFUNCTION(BlueprintCallable, Category = "Combat")
	bool SwitchToBestCombatWeapon();

	// Convenience: does the bot have any weapon at all?
	UFUNCTION(BlueprintPure, Category = "Combat")
	bool HasAnyWeapon() const { return WeaponInventory.Num() > 0; }
	
	UFUNCTION(BlueprintPure, Category = "Combat")
    AWeaponBase* GetWeaponInSlot(int32 SlotIndex) const;
	
	// Max weapons this character can carry
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	int32 MaxWeaponSlots = 4;

	// Would this character benefit from a weapon or ammo pickup?
	// Returns true if:
	//   - We have room for another weapon, OR
	//   - Any owned non-infinite weapon is empty
	UFUNCTION(BlueprintPure, Category = "Combat")
	bool WantsWeaponOrAmmoPickup(TSubclassOf<class AWeaponBase> PickupClass) const;
protected:
	virtual void BeginPlay() override;

	UPROPERTY(BlueprintReadOnly, Category = "Combat")
	TArray<AWeaponBase*> WeaponInventory;

	UPROPERTY()
	AWeaponBase* CurrentWeapon;

	int32 CurrentWeaponIndex = -1;

	UPROPERTY()
	ABaseCharacter* OwnerCharacter;

	//void InitializeWeapons();
	
	UPROPERTY()
	USceneComponent* CachedAttachPoint;
	
	void CacheAttachPoint();
	
	
};