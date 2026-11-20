#include "PauseMenuWidget.h"
#include "LeaderboardWidget.h"
#include "MyPlayerController.h"
#include "Components/Button.h"
#include "Kismet/GameplayStatics.h"
#include "MatchDataSubsystem.h"
#include "Runtime/Engine/Classes/Kismet/GameplayStatics.h"

void UPauseMenuWidget::NativeConstruct()
{
	Super::NativeConstruct();

	// Bind button clicks
	if (Btn_Resume) Btn_Resume->OnClicked.AddDynamic(this, &UPauseMenuWidget::OnResumeClicked);
	if (Btn_Restart) Btn_Restart->OnClicked.AddDynamic(this, &UPauseMenuWidget::OnRestartClicked);
	if (Btn_QuitToTitle) Btn_QuitToTitle->OnClicked.AddDynamic(this, &UPauseMenuWidget::OnQuitClicked);

	// Populate the leaderboard immediately when the pause menu opens
	if (Leaderboard)
    	{
    		UE_LOG(LogTemp, Warning, TEXT("PauseMenu: Leaderboard found. Calling UpdateLeaderboard."));
    		Leaderboard->UpdateLeaderboard();
    	}
    	else
    	{
    		UE_LOG(LogTemp, Error, TEXT("PauseMenu: Leaderboard is NULL. Check the widget name in UMG."));
    	}
}

void UPauseMenuWidget::OnResumeClicked()
{
	// Tell the Player Controller to unpause and remove this UI
	if (AMyPlayerController* PC = Cast<AMyPlayerController>(GetOwningPlayer()))
	{
		PC->SetGameFlowState(EGameFlowState::Playing);
	}
}

void UPauseMenuWidget::OnRestartClicked()
{
	// Clear stats and reload level
	if (UGameInstance* GI = GetGameInstance())
	{
		if (UMatchDataSubsystem* Subsystem = GI->GetSubsystem<UMatchDataSubsystem>())
		{
			Subsystem->ClearMatchResults();
		}
	}

	UGameplayStatics::SetGamePaused(GetWorld(), false); // Must unpause before loading!
	UGameplayStatics::OpenLevel(this, FName(*UGameplayStatics::GetCurrentLevelName(this)));
}

void UPauseMenuWidget::OnQuitClicked()
{
	// Clear stats and go to Title Screen
	if (UGameInstance* GI = GetGameInstance())
	{
		if (UMatchDataSubsystem* Subsystem = GI->GetSubsystem<UMatchDataSubsystem>())
		{
			Subsystem->ClearMatchResults();
		}
	}

	UGameplayStatics::SetGamePaused(GetWorld(), false); // Must unpause before loading!
	UGameplayStatics::OpenLevel(this, FName("TitleMap")); // Replace with your title map name
}