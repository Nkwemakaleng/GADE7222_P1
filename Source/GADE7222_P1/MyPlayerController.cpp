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