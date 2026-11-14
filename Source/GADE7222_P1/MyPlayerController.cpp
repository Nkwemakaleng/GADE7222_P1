#include "MyPlayerController.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "PlayerCharacter.h"
#include "Engine/LocalPlayer.h"
#include "PauseMenuWidget.h"
#include "Kismet/GameplayStatics.h"


AMyPlayerController::AMyPlayerController()
{
	PrimaryActorTick.bCanEverTick = true;
}

void AMyPlayerController::BeginPlay()
{
	Super::BeginPlay();

	// Add the Input Mapping Context to the local player subsystem
	if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
	{
		if (InputMappingContext)
		{
			Subsystem->AddMappingContext(InputMappingContext, 0);
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("MyPlayerController: InputMappingContext is null! Please assign it in the Blueprint."));
		}
	}
}

void AMyPlayerController::SetGameFlowState(EGameFlowState NewState)
{
	CurrentState = NewState;

	switch (CurrentState)
	{
	case EGameFlowState::Playing:
		SetInputMode(FInputModeGameOnly());
		bShowMouseCursor = false;
		UGameplayStatics::SetGamePaused(GetWorld(), false);
		if (PauseMenuInstance) PauseMenuInstance->RemoveFromParent();
		break;

	case EGameFlowState::Paused:
		SetInputMode(FInputModeGameAndUI());
		bShowMouseCursor = true;
		UGameplayStatics::SetGamePaused(GetWorld(), true);
		if (PauseMenuClass && !PauseMenuInstance)
		{
			PauseMenuInstance = CreateWidget<UPauseMenuWidget>(this, PauseMenuClass);
			PauseMenuInstance->AddToViewport();
		}
		break;
		
	case EGameFlowState::MatchOver:
	case EGameFlowState::TitleScreen:
		SetInputMode(FInputModeUIOnly());
		bShowMouseCursor = true;
		break;
	}
}

void AMyPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	/*// Pass the Input Actions from the Controller to the Player Character
	if (APlayerCharacter* PlayerCharacter = Cast<APlayerCharacter>(GetPawn()))
	{
		PlayerCharacter->MoveForwardAction = MoveForwardAction;
		PlayerCharacter->MoveRightAction = MoveRightAction;
		PlayerCharacter->LookAction = LookAction;
		PlayerCharacter->JumpAction = JumpAction;
		PlayerCharacter->CrouchAction = CrouchAction;
		
		// Pass Combat Actions
		PlayerCharacter->FireAction = FireAction;
		PlayerCharacter->SwitchWeaponAction = SwitchWeaponAction;
	}*/
}

void AMyPlayerController::TogglePauseMenu()
{
	if (CurrentState == EGameFlowState::Playing)
		SetGameFlowState(EGameFlowState::Paused);
	else if (CurrentState == EGameFlowState::Paused)
		SetGameFlowState(EGameFlowState::Playing);
}