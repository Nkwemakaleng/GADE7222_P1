#pragma once

#include "CoreMinimal.h"
#include "WidgetBase.h"
#include "Blueprint/UserWidget.h"
#include "LeaderboardWidget.generated.h"

class UVerticalBox;
class ULeaderboardRowWidget;

UCLASS()
class GADE7222_P1_API ULeaderboardWidget : public UWidgetBase
{
	GENERATED_BODY()

public:
	// Call this to refresh the board with the latest data
	UFUNCTION(BlueprintCallable, Category = "UI")
	void UpdateLeaderboard();

protected:
	// Bind the VerticalBox from UMG
	UPROPERTY(meta = (BindWidget))
	UVerticalBox* RowContainer;

	// Assign the WBP_LeaderboardRow class here (Edit Defaults in the Blueprint)
	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<ULeaderboardRowWidget> RowWidgetClass;
};