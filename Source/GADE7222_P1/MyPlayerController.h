#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "MyGameState.h"
#include "MyPlayerController.generated.h"

UCLASS()
class GADE7222_P1_API AMyPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	AMyPlayerController();
	
	UFUNCTION(BlueprintCallable, Category = "Flow")
	void SetGameFlowState(EGameFlowState NewState);

protected:
	virtual void BeginPlay() override;
	virtual void SetupInputComponent() override;
	
	// Enhanced Input function
	void TogglePauseMenu();

	UPROPERTY(BlueprintReadOnly, Category = "Flow")
	EGameFlowState CurrentState = EGameFlowState::Playing;

	// UI Classes
	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<class UPauseMenuWidget> PauseMenuClass;
	UPROPERTY() class UPauseMenuWidget* PauseMenuInstance;

public:
	// Input mapping context
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	class UInputMappingContext* InputMappingContext;

	// Input actions 
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
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	class UInputAction* FireAction;
	//swap weapons with the mouse wheel
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	class UInputAction* SwitchWeaponAction;
	
	//num1 and 2 for the weapon slots
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	class UInputAction* WeaponSlot1Action;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	class UInputAction* WeaponSlot2Action;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	class UInputAction* SprintAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	class UInputAction* InteractAction;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
    class UInputAction* DashAction;
};