#include "MatchOverWidget.h"
#include "LeaderboardWidget.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "Kismet/GameplayStatics.h"
#include "MatchDataSubsystem.h"

void UMatchOverWidget::NativeConstruct()
{
	Super::NativeConstruct();

	// Set the title text
	if (Text_MatchOver)
	{
		Text_MatchOver->SetText(FText::FromString(TEXT("MATCH OVER")));
	}

	// Populate the leaderboard
	if (Leaderboard)
	{
		Leaderboard->UpdateLeaderboard();
	}

	// Bind buttons
	if (Btn_Restart) Btn_Restart->OnClicked.AddDynamic(this, &UMatchOverWidget::OnRestartClicked);
	if (Btn_Quit) Btn_Quit->OnClicked.AddDynamic(this, &UMatchOverWidget::OnQuitClicked);
}

void UMatchOverWidget::OnRestartClicked()
{
	// Clear the match data for the new game
	if (UGameInstance* GI = GetGameInstance())
	{
		if (UMatchDataSubsystem* Subsystem = GI->GetSubsystem<UMatchDataSubsystem>())
		{
			Subsystem->ClearMatchResults();
		}
	}

	// Unpause the game, then reload the current level
	UGameplayStatics::SetGamePaused(GetWorld(), false);
	UGameplayStatics::OpenLevel(this, FName(*UGameplayStatics::GetCurrentLevelName(this)));
}

void UMatchOverWidget::OnQuitClicked()
{
	// Unpause the game, then go back to the Main Menu
	UGameplayStatics::SetGamePaused(GetWorld(), false);
	UGameplayStatics::OpenLevel(this, FName("MainMenuMap")); 
}