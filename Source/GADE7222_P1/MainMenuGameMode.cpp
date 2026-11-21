#include "MainMenuGameMode.h"
#include "MyPlayerController.h"

AMainMenuGameMode::AMainMenuGameMode()
{
	// Do not spawn a default pawn in the main menu
	DefaultPawnClass = nullptr;
	
	//Custom Player Controller
	PlayerControllerClass = AMyPlayerController::StaticClass();
}