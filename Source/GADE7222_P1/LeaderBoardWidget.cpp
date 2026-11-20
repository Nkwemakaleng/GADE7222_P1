#include "LeaderboardWidget.h"
#include "LeaderboardRowWidget.h"
#include "GamePlayerState.h" 
#include "Components/VerticalBox.h"
#include "GameFramework/GameStateBase.h"

void ULeaderboardWidget::UpdateLeaderboard()
{
	
	if (!RowContainer) 
	{
		UE_LOG(LogTemp, Error, TEXT("Leaderboard: RowContainer is NULL. Check BindWidget name in UMG."));
		return;
	}
	
	if (!RowWidgetClass) 
	{
		UE_LOG(LogTemp, Error, TEXT("Leaderboard: RowWidgetClass is NULL. Assign WBP_LeaderboardRow in Class Defaults."));
		return;
	}

	// 1. Clear existing rows
	RowContainer->ClearChildren();

	// 2. Get the GameState
	AGameStateBase* GS = GetWorld()->GetGameState();
	if (!GS) return;

	// 3. Gather PlayerStates
	TArray<AGamePlayerState*> PlayerStates;
	for (APlayerState* PS : GS->PlayerArray)
	{
		AGamePlayerState* MyPS = Cast<AGamePlayerState>(PS);
		if (MyPS)
		{
			PlayerStates.Add(MyPS);
		}
	}

	// 4. Sort by Kills (Descending), then Deaths (Ascending)
	PlayerStates.Sort([](const AGamePlayerState& A, const AGamePlayerState& B) {
		if (A.Kills == B.Kills)
		{
			return A.Deaths < B.Deaths;
		}
		return A.Kills > B.Kills;
	});

	// 5. Spawn Rows
	int32 Position = 1;
	for (AGamePlayerState* PS : PlayerStates)
	{
		ULeaderboardRowWidget* NewRow = CreateWidget<ULeaderboardRowWidget>(this, RowWidgetClass);
		if (NewRow)
		{
			// Check if this PlayerState belongs to the local player
			bool bIsLocal = (PS->GetPlayerController() != nullptr && PS->GetPlayerController()->IsLocalController());

			NewRow->SetRowData(Position, PS->GetPlayerName(), PS->Kills, PS->Deaths, bIsLocal);
			RowContainer->AddChildToVerticalBox(NewRow);
			Position++;
		}
	}
	
	UE_LOG(LogTemp, Warning, TEXT("Leaderboard Updated with %d players."), PlayerStates.Num());
}