#pragma once
#include "CoreMinimal.h"
#include "MyGameState.generated.h"

UENUM(BlueprintType)
enum class EGameFlowState : uint8
{
	TitleScreen,
	Playing,
	Paused,
	MatchOver
};