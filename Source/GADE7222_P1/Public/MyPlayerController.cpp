// MyPlayerController.cpp
#include "MyPlayerController.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "PlayerCharacter.h"
#include "Engine/LocalPlayer.h"

AMyPlayerController::AMyPlayerController()
{
	PrimaryActorTick.bCanEverTick = true;
}

void AMyPlayerController::BeginPlay()
{
	Super::BeginPlay();

	// Add Input Mapping Context
	if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
	{
		EnhancedInputSubsystem = Subsystem;
		
		if (InputMappingContext)
		{
			Subsystem->AddMappingContext(InputMappingContext, 0);
			UE_LOG(LogTemp, Warning, TEXT("Input Mapping Context added successfully"));
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("Input Mapping Context is null! Please assign it in the Blueprint."));
		}
	}
}

void AMyPlayerController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);
}

void AMyPlayerController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AMyPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	if (UEnhancedInputComponent* EnhancedInput = Cast<UEnhancedInputComponent>(InputComponent))
	{
		if (APlayerCharacter* PlayerCharacter = Cast<APlayerCharacter>(GetPawn()))
		{
			// Assign actions to the player character
			PlayerCharacter->MoveForwardAction = MoveForwardAction;
			PlayerCharacter->MoveRightAction = MoveRightAction;
			PlayerCharacter->LookAction = LookAction;
			PlayerCharacter->JumpAction = JumpAction;
			PlayerCharacter->CrouchAction = CrouchAction;
		}
	}
}