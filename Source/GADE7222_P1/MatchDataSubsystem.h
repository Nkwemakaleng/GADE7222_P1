#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "MatchDataSubsystem.generated.h"

USTRUCT(BlueprintType)
struct FMatchResult
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly) FString PlayerName;
	UPROPERTY(BlueprintReadOnly) int32 Kills;
	UPROPERTY(BlueprintReadOnly) bool bIsLocalPlayer;
};

UCLASS()
class GADE7222_P1_API UMatchDataSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadOnly, Category = "Match")
	TArray<FMatchResult> LastMatchResults;

	void SaveMatchResults(const TArray<FMatchResult>& Results) { LastMatchResults = Results; }
	void ClearMatchResults() { LastMatchResults.Empty(); }
};