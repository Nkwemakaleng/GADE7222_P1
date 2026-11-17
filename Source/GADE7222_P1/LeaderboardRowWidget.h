#pragma once

#include "CoreMinimal.h"
#include "WidgetBase.h"
#include "Blueprint/UserWidget.h"
#include "LeaderboardRowWidget.generated.h"

class UTextBlock;

UCLASS()
class GADE7222_P1_API ULeaderboardRowWidget : public UWidgetBase
{
	GENERATED_BODY()

public:
	// Call this to populate the row
	UFUNCTION(BlueprintCallable, Category = "UI")
	void SetRowData(int32 Position, const FString& Name, int32 Kills, int32 Deaths, bool bIsLocalPlayer);

protected:
	UPROPERTY(meta = (BindWidget))
	UTextBlock* Text_Position;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* Text_Name;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* Text_Kills;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* Text_Deaths;
};