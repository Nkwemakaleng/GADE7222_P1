#pragma once

#include "CoreMinimal.h"
#include "WidgetBase.h"
#include "Blueprint/UserWidget.h"
#include "LeaderboardBoxWidget.generated.h"

class UVerticalBox;
class ULeaderboardRowWidget;

UCLASS()
class GADE7222_P1_API ULeaderboardBoxWidget : public UWidgetBase
{
	GENERATED_BODY()

public:
	// Call this to refresh the leaderboard
	UFUNCTION(BlueprintCallable, Category = "UI")
	void UpdateLeaderboard();

protected:
	// The box that holds all the rows (Bind in UMG)
	UPROPERTY(meta = (BindWidget))
	UVerticalBox* RowContainer;

	// The Row Widget class to spawn (Assign in UMG defaults)
	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<ULeaderboardRowWidget> RowWidgetClass;
};