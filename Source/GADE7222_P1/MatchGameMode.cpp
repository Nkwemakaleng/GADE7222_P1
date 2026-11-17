#include "MatchGameMode.h"
#include "GamePlayerState.h"
#include "MatchDataSubsystem.h"
#include "MyGameState.h"
#include "Kismet/GameplayStatics.h"
#include "Blueprint/UserWidget.h"
#include "TimerManager.h"
#include "GameFramework/GameStateBase.h" 

AMatchGameMode::AMatchGameMode()
{
	PlayerStateClass = AGamePlayerState::StaticClass();
	GameStateClass = AMyGameState::StaticClass();
	TimeRemaining = MatchDuration;
}

void AMatchGameMode::BeginPlay()
{
	Super::BeginPlay();
	TimeRemaining = MatchDuration;

	GetWorldTimerManager().SetTimer(MatchTimerHandle, this, &AMatchGameMode::UpdateTimer, 1.0f, true);
}

void AMatchGameMode::UpdateTimer()
{
	if (TimeRemaining > 0)
	{
		TimeRemaining--;
	}
	else
	{
		GetWorldTimerManager().ClearTimer(MatchTimerHandle);
		EndMatch();
	}
}

void AMatchGameMode::EndMatch()
{
	TArray<FMatchResult> Results;
	
	// Safety check to ensure GameState exists
	if (GameState)
	{
		for (APlayerState* PS : GameState->PlayerArray)
		{
			AGamePlayerState* MyPS = Cast<AGamePlayerState>(PS);
			if (MyPS)
			{
				FMatchResult Result;
				Result.PlayerName = MyPS->GetPlayerName(); // This now uses our DisplayName
				Result.Kills = MyPS->Kills;
				Result.Deaths = MyPS->Deaths;
				Result.bIsLocalPlayer = (MyPS->GetPlayerController() != nullptr && MyPS->GetPlayerController()->IsLocalController());
				Results.Add(Result);
			}
		}
	}

	Results.Sort([](const FMatchResult& A, const FMatchResult& B) {
		if (A.Kills == B.Kills)
		{
			return A.Deaths < B.Deaths; // Tie-breaker: fewer deaths is better
		}
		return A.Kills > B.Kills; // Most kills is better
	});
	

	if (UGameInstance* GI = GetGameInstance())
	{
		if (UMatchDataSubsystem* Subsystem = GI->GetSubsystem<UMatchDataSubsystem>())
		{
			Subsystem->SaveMatchResults(Results);
		}
	}

	UGameplayStatics::SetGamePaused(GetWorld(), true);
	// Trigger Blueprint event or show UI here
}