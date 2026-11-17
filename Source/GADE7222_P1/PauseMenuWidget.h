#pragma once

#include "CoreMinimal.h"
#include "WidgetBase.h"
#include "Blueprint/UserWidget.h"
#include "PauseMenuWidget.generated.h"

class UButton;
class ULeaderboardWidget;

UCLASS()
class GADE7222_P1_API UPauseMenuWidget : public UWidgetBase
{
	GENERATED_BODY()

protected:
	virtual void NativeConstruct() override;

	// Bind buttons from UMG
	UPROPERTY(meta = (BindWidget))
	UButton* Btn_Resume;

	UPROPERTY(meta = (BindWidget))
	UButton* Btn_Restart;

	UPROPERTY(meta = (BindWidget))
	UButton* Btn_QuitToTitle;

	// Bind the Leaderboard widget from UMG
	UPROPERTY(meta = (BindWidget))
	ULeaderboardWidget* Leaderboard;

	UFUNCTION() void OnResumeClicked();
	UFUNCTION() void OnRestartClicked();
	UFUNCTION() void OnQuitClicked();
};