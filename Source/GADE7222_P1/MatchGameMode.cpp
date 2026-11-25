#include "MatchGameMode.h"
#include "GamePlayerState.h"
#include "MatchDataSubsystem.h"
#include "MatchOverWidget.h" 
#include "MyPlayerController.h"
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
	//Prevents EndMatch from being called twice
	if (UGameplayStatics::IsGamePaused(GetWorld())) return;
	// 1. Gather all player states and save to subsystem (for the leaderboard)
	TArray<FMatchResult> Results;
	
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
				Result.Deaths = MyPS->Deaths;
				
				AMyPlayerController* PC = Cast<AMyPlayerController>(MyPS->GetPlayerController());
				Result.bIsLocalPlayer = (PC != nullptr && PC->IsLocalController());
				
				Results.Add(Result);
			}
		}
	}

	Results.Sort([](const FMatchResult& A, const FMatchResult& B) {
		if (A.Kills == B.Kills) return A.Deaths < B.Deaths;
		return A.Kills > B.Kills;
	});

	if (UGameInstance* GI = GetGameInstance())
	{
		if (UMatchDataSubsystem* Subsystem = GI->GetSubsystem<UMatchDataSubsystem>())
		{
			Subsystem->SaveMatchResults(Results);
		}
	}

	// 2. Pause the game
	UGameplayStatics::SetGamePaused(GetWorld(), true);

	// 3. Spawn the Match Over UI
	if (AMyPlayerController* PC = Cast<AMyPlayerController>(UGameplayStatics::GetPlayerController(this, 0)))
	{
		if (MatchOverWidgetClass)
		{
			UMatchOverWidget* MatchOverWidget = CreateWidget<UMatchOverWidget>(PC, MatchOverWidgetClass);
			if (MatchOverWidget)
			{
				MatchOverWidget->AddToViewport();
				
				PC->SetGameFlowState(EGameFlowState::MatchOver);
				PC->bShowMouseCursor = true;
				FInputModeUIOnly InputMode;
				InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
				PC->SetInputMode(InputMode);
			}
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("MatchGameMode: MatchOverWidgetClass is NULL! Assign it in BP_MatchGameMode."));
		}
	}
}