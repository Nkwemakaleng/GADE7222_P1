#pragma once

#include "CoreMinimal.h"
#include "BaseCharacter.h"
#include "PlayerCharacter.generated.h"

UCLASS()
class GADE7222_P1_API APlayerCharacter : public ABaseCharacter
{
	GENERATED_BODY()

public:
	APlayerCharacter();

	// --- Input Action references  ---
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	class UInputAction* MoveForwardAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	class UInputAction* MoveRightAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	class UInputAction* LookAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	class UInputAction* JumpAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	class UInputAction* CrouchAction;

	// --- Combat Input Actions ---
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	class UInputAction* FireAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	class UInputAction* SwitchWeaponAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	class UInputAction* WeaponSlot1Action;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	class UInputAction* WeaponSlot2Action;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	class UInputAction* SprintAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	class UInputAction* InteractAction;
	
	//------------Hud Properties--------------
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "UI")
	TSubclassOf<class UHUDWidget> HUDWidgetClass;

	UPROPERTY()
	class UHUDWidget* HUDWidget;
	
	//------------Dash Properties--------------
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	class UInputAction* DashAction;
protected:
	virtual void BeginPlay() override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	// Input functions (these can stay protected since only this class calls them)
	void MoveForwardInput(const struct FInputActionValue& Value);
	void MoveRightInput(const struct FInputActionValue& Value);
	void JumpInput(const struct FInputActionValue& Value);
	void CrouchInput(const struct FInputActionValue& Value);
	void LookInput(const struct FInputActionValue& Value);
	void FireInput(const struct FInputActionValue& Value);
	void SwitchWeaponInput(const struct FInputActionValue& Value);
	void WeaponSlot1Input(const struct FInputActionValue& Value);
	void WeaponSlot2Input(const struct FInputActionValue& Value);
	void SprintInput(const struct FInputActionValue& Value);
	void InteractInput(const struct FInputActionValue& Value);
	void DashInput(const struct FInputActionValue& Value);
};