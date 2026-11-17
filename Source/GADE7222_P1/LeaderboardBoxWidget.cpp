#include "LeaderboardBoxWidget.h"
#include "LeaderboardRowWidget.h"
#include "MatchDataSubsystem.h"
#include "Components/VerticalBox.h"
#include "Engine/GameInstance.h"

void ULeaderboardBoxWidget::UpdateLeaderboard()
{
	if (!RowContainer || !RowWidgetClass) return;

	// 1. Clear existing rows
	RowContainer->ClearChildren();

	// 2. Get the saved results
	UGameInstance* GI = GetGameInstance();
	if (!GI) return;
	UMatchDataSubsystem* Subsystem = GI->GetSubsystem<UMatchDataSubsystem>();
	if (!Subsystem) return;

	// 3. Spawn a new row for each result
	int32 Position = 1;
	for (const FMatchResult& Result : Subsystem->LastMatchResults)
	{
		ULeaderboardRowWidget* NewRow = CreateWidget<ULeaderboardRowWidget>(this, RowWidgetClass);
		if (NewRow)
		{
			NewRow->SetRowData(Position, Result.PlayerName, Result.Kills, Result.Deaths, Result.bIsLocalPlayer);
			RowContainer->AddChildToVerticalBox(NewRow);
			Position++;
		}
	}
}