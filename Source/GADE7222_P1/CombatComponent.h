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
	bool HasWeaponInSlot(int32 SlotIndex) const
	{
		return WeaponInventory.IsValidIndex(SlotIndex);
	}
protected:
	virtual void BeginPlay() override;

	UPROPERTY()
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