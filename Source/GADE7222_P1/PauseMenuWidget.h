#pragma once

#include "CoreMinimal.h"
#include "WidgetBase.h"
#include "WidgetBase.h"
#include "PauseMenuWidget.generated.h"

class UButton;
class ULeaderboardWidget;

UCLASS()
class GADE7222_P1_API UPauseMenuWidget : public UWidgetBase
{
	GENERATED_BODY()

protected:
	virtual void NativeConstruct() override;

	// Bind these in the UMG Designer (names must match exactly)
	UPROPERTY(meta = (BindWidgetOptional))
	class ULeaderboardWidget* Leaderboard;

	UPROPERTY(meta = (BindWidgetOptional))
	class UButton* Btn_Resume;

	UPROPERTY(meta = (BindWidgetOptional))
	class UButton* Btn_Restart;

	UPROPERTY(meta = (BindWidgetOptional))
	class UButton* Btn_QuitToTitle;

	UFUNCTION()
	void OnResumeClicked();

	UFUNCTION()
	void OnRestartClicked();

	UFUNCTION()
	void OnQuitClicked();
};