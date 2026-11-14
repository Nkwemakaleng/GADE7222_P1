#include "MatchGameMode.h"
#include "GamePlayerState.h"
#include "MatchDataSubsystem.h"
#include "Kismet/GameplayStatics.h"
#include "Blueprint/UserWidget.h"
#include "TimerManager.h"
#include "GameFramework/GameStateBase.h" 

AMatchGameMode::AMatchGameMode()
{
	PlayerStateClass = AGamePlayerState::StaticClass();
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
				Result.PlayerName = MyPS->GetPlayerName();
				Result.Kills = MyPS->Kills;
				
				// --- FIX: Check for Local Player correctly ---
				APlayerController* PC = MyPS->GetPlayerController();
				Result.bIsLocalPlayer = (PC != nullptr && PC->IsLocalController());
				
				Results.Add(Result);
			}
		}
	}

	// Sort by kills (Descending)
	Results.Sort([](const FMatchResult& A, const FMatchResult& B) {
		return A.Kills > B.Kills;
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