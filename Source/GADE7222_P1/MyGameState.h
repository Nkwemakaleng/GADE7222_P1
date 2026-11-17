#pragma once
#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "MyGameState.generated.h"

UENUM(BlueprintType)
enum class EGameFlowState : uint8
{
	TitleScreen,
	Playing,
	Paused,
	MatchOver
};

// Delegate: Killer Name, Victim Name, Weapon Name
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnKillFeedSignature, const FString&, KillerName, const FString&, VictimName, const FString&, WeaponName);

UCLASS()
class GADE7222_P1_API AMyGameState : public AGameStateBase
{
	GENERATED_BODY()

public:
	AMyGameState();

	// The HUD will bind to this event
	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnKillFeedSignature OnKillFeedEvent;

	// Called by the character when they die
	UFUNCTION(BlueprintCallable, Category = "Events")
	void BroadcastKill(const FString& KillerName, const FString& VictimName, const FString& WeaponName);
};