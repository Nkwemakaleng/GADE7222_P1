#include "PauseMenuWidget.h"
#include "LeaderBoardWidget.h"
#include "MyPlayerController.h"
#include "Components/Button.h"
#include "Kismet/GameplayStatics.h"
#include "MatchDataSubsystem.h"

void UPauseMenuWidget::NativeConstruct()
{
	Super::NativeConstruct();

	// Bind the buttons to their functions
	if (Btn_Resume)
	{
		Btn_Resume->OnClicked.AddDynamic(this, &UPauseMenuWidget::OnResumeClicked);
	}
	if (Btn_Restart)
	{
		Btn_Restart->OnClicked.AddDynamic(this, &UPauseMenuWidget::OnRestartClicked);
	}
	if (Btn_QuitToTitle)
	{
		Btn_QuitToTitle->OnClicked.AddDynamic(this, &UPauseMenuWidget::OnQuitClicked);
	}

	// Populate the leaderboard when the pause menu opens
	if (Leaderboard)
	{
		Leaderboard->PopulateBoard();
	}
}

void UPauseMenuWidget::OnResumeClicked()
{
	// Tell the Player Controller to unpause the game and remove this widget
	if (AMyPlayerController* PC = Cast<AMyPlayerController>(GetOwningPlayer()))
	{
		PC->SetGameFlowState(EGameFlowState::Playing);
	}
}

void UPauseMenuWidget::OnRestartClicked()
{
	// Clear scores and restart the current level
	if (UGameInstance* GI = GetGameInstance())
	{
		if (UMatchDataSubsystem* Subsystem = GI->GetSubsystem<UMatchDataSubsystem>())
		{
			Subsystem->ClearMatchResults();
		}
	}

	UGameplayStatics::SetGamePaused(GetWorld(), false); // Unpause before loading
	UGameplayStatics::OpenLevel(this, FName(*UGameplayStatics::GetCurrentLevelName(this)));
}

void UPauseMenuWidget::OnQuitClicked()
{
	// Clear scores and load the Title Screen level
	if (UGameInstance* GI = GetGameInstance())
	{
		if (UMatchDataSubsystem* Subsystem = GI->GetSubsystem<UMatchDataSubsystem>())
		{
			Subsystem->ClearMatchResults();
		}
	}

	UGameplayStatics::SetGamePaused(GetWorld(), false); // Unpause before loading
	UGameplayStatics::OpenLevel(this, FName("TitleMap")); // Make sure your Title map is named TitleMap!
}