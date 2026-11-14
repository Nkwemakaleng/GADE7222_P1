#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "MatchGameMode.generated.h"

UCLASS()
class GADE7222_P1_API AMatchGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	AMatchGameMode();

protected:
	virtual void BeginPlay() override;
	void UpdateTimer();
	void EndMatch();

	FTimerHandle MatchTimerHandle;
	
	UPROPERTY(EditDefaultsOnly, Category = "Match")
	float MatchDuration = 300.0f; // 5 minutes in seconds

	int32 TimeRemaining;

public:
	UFUNCTION(BlueprintPure, Category = "Match")
	int32 GetTimeRemaining() const { return TimeRemaining; }
};