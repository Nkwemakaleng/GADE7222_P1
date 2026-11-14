#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "GamePlayerState.generated.h"

UCLASS()
class GADE7222_P1_API AGamePlayerState : public APlayerState
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadOnly, Category = "Match")
	int32 Kills = 0;

	UFUNCTION(BlueprintCallable, Category = "Match")
	void AddKill() { Kills++; }
	
	UFUNCTION(BlueprintCallable, Category = "Match")
	void ResetKills() { Kills = 0; }
};