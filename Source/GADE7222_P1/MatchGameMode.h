#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "MatchGameMode.generated.h"

class UMatchOverWidget; 

UCLASS()
class GADE7222_P1_API AMatchGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	AMatchGameMode();

	
	UFUNCTION(BlueprintCallable, Category = "Match")
	void EndMatch();

protected:
	virtual void BeginPlay() override;
	void UpdateTimer();

	FTimerHandle MatchTimerHandle;
	
	UPROPERTY(EditDefaultsOnly, Category = "Match")
	float MatchDuration = 300.0f;

	int32 TimeRemaining;

	
	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<UMatchOverWidget> MatchOverWidgetClass;

public:
	UFUNCTION(BlueprintPure, Category = "Match")
	int32 GetTimeRemaining() const { return TimeRemaining; }
};