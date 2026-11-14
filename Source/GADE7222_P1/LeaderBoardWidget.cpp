#include "LeaderBoardWidget.h"
#include "MatchDataSubsystem.h"
#include "Components/ListView.h"
#include "Engine/GameInstance.h"
#include "Kismet/GameplayStatics.h"

void ULeaderboardWidget::PopulateBoard()
{
	// 1. Get the MatchDataSubsystem
	UGameInstance* GI = GetGameInstance();
	if (!GI) return;

	UMatchDataSubsystem* MatchSubsystem = GI->GetSubsystem<UMatchDataSubsystem>();
	if (!MatchSubsystem) return;

	// 2. If you are using a UListView in your Blueprint, clear the old items and add the new ones.
	// NOTE: You will need to create a custom UObject wrapper class for the ListView items in a real setup. 
	// For now, this function acts as the logic hub. You can bind this to a Blueprint node and manually spawn text blocks.
	
	// (If using a VerticalBox instead of ListView, you would clear children and create text blocks here)
	
	// Example of how you would log the data to verify it works:
	UE_LOG(LogTemp, Warning, TEXT("--- LEADERBOARD ---"));
	for (const FMatchResult& Result : MatchSubsystem->LastMatchResults)
	{
		UE_LOG(LogTemp, Warning, TEXT("%s: %d Kills"), *Result.PlayerName, Result.Kills);
	}
}