#pragma once

#include "CoreMinimal.h"
#include "WidgetBase.h"
#include "Blueprint/UserWidget.h"
#include "MatchOverWidget.generated.h"

class UButton;
class UTextBlock;
class ULeaderboardWidget;

UCLASS()
class GADE7222_P1_API UMatchOverWidget : public UWidgetBase
{
	GENERATED_BODY()

protected:
	virtual void NativeConstruct() override;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* Text_MatchOver;

	UPROPERTY(meta = (BindWidget))
	ULeaderboardWidget* Leaderboard;
	
	UPROPERTY(meta = (BindWidget))
	UButton* Btn_Restart;

	UPROPERTY(meta = (BindWidget))
	UButton* Btn_Quit;

	UFUNCTION()
	void OnRestartClicked();

	UFUNCTION()
	void OnQuitClicked();
};