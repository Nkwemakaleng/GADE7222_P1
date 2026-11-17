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

void AMyPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	if (UEnhancedInputComponent* EnhancedInput = Cast<UEnhancedInputComponent>(InputComponent))
	{
		
		if (PauseAction)
		{
			EnhancedInput->BindAction(PauseAction, ETriggerEvent::Started, this, &AMyPlayerController::TogglePauseMenu);
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("MyPlayerController: PauseAction is null! Please assign it in the Blueprint."));
		}
	}
}

void AMyPlayerController::TogglePauseMenu()
{
	if (CurrentState == EGameFlowState::Playing)
	{
		SetGameFlowState(EGameFlowState::Paused);
	}
	else if (CurrentState == EGameFlowState::Paused)
	{
		SetGameFlowState(EGameFlowState::Playing);
	}
}

void AMyPlayerController::SetGameFlowState(EGameFlowState NewState)
{
	CurrentState = NewState;

	switch (CurrentState)
	{
	case EGameFlowState::Playing:
		// Hide menu and unpause
		if (PauseMenuInstance)
		{
			PauseMenuInstance->RemoveFromParent();
			PauseMenuInstance = nullptr;
		}
		UGameplayStatics::SetGamePaused(GetWorld(), false);
		bShowMouseCursor = false;
		SetInputMode(FInputModeGameOnly());
		break;

	case EGameFlowState::Paused:
		{
			
			// Show menu and pause
			if (PauseMenuClass && !PauseMenuInstance)
			{
				PauseMenuInstance = CreateWidget<UPauseMenuWidget>(this, PauseMenuClass);
				if (PauseMenuInstance)
				{
					PauseMenuInstance->AddToViewport();
				}
			}
			UGameplayStatics::SetGamePaused(GetWorld(), true);
			bShowMouseCursor = true;
		
			FInputModeGameAndUI InputMode;
			InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
			InputMode.SetHideCursorDuringCapture(false);
			SetInputMode(InputMode);
			break;
		} 

	case EGameFlowState::MatchOver:
	case EGameFlowState::TitleScreen:
		bShowMouseCursor = true;
		SetInputMode(FInputModeUIOnly());
		break;
	}
}