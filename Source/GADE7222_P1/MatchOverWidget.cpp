#include "MatchOverWidget.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "Kismet/GameplayStatics.h"
#include "MatchDataSubsystem.h"

void UMatchOverWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (Text_MatchOver)
	{
		Text_MatchOver->SetText(FText::FromString(TEXT("MATCH OVER")));
	}

	if (Btn_Restart) Btn_Restart->OnClicked.AddDynamic(this, &UMatchOverWidget::OnRestartClicked);
	if (Btn_Quit) Btn_Quit->OnClicked.AddDynamic(this, &UMatchOverWidget::OnQuitClicked);
}

void UMatchOverWidget::OnRestartClicked()
{
	// Clear the leaderboard data for the new match
	if (UGameInstance* GI = GetGameInstance())
	{
		if (UMatchDataSubsystem* Subsystem = GI->GetSubsystem<UMatchDataSubsystem>())
		{
			Subsystem->ClearMatchResults();
		}
	}

	// Unpause and reload the current level
	UGameplayStatics::SetGamePaused(GetWorld(), false);
	UGameplayStatics::OpenLevel(this, FName(*UGameplayStatics::GetCurrentLevelName(this)));
}

void UMatchOverWidget::OnQuitClicked()
{
	// Unpause and return to Main Menu
	UGameplayStatics::SetGamePaused(GetWorld(), false);
	UGameplayStatics::OpenLevel(this, FName("MainMenuMap")); // Replace with your Main Menu map name
}