#pragma once

#include "CoreMinimal.h"
#include "WidgetBase.h"
#include "Blueprint/UserWidget.h"
#include "LeaderBoardWidget.generated.h"

class UListView;

UCLASS()
class GADE7222_P1_API ULeaderboardWidget : public UWidgetBase
{
	GENERATED_BODY()

public:
	// Call this to populate the board
	UFUNCTION(BlueprintCallable, Category = "UI")
	void PopulateBoard();

protected:
	// UListView in UMG (you must add this in the Blueprint designer and name it exactly PlayerListView)
	UPROPERTY(meta = (BindWidgetOptional))
	class UListView* PlayerListView;
};