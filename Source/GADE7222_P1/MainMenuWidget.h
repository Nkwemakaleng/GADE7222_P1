#pragma once

#include "CoreMinimal.h"
#include "WidgetBase.h"
#include "Blueprint/UserWidget.h"
#include "MainMenuWidget.generated.h"

class UButton;
class USlider;
class UWidgetSwitcher;

UCLASS()
class GADE7222_P1_API UMainMenuWidget : public UWidgetBase
{
	GENERATED_BODY()

public:
	// Assign these in the UMG Blueprint defaults
	UPROPERTY(EditDefaultsOnly, Category = "Audio")
	class USoundMix* MasterSoundMix;

	UPROPERTY(EditDefaultsOnly, Category = "Audio")
	class USoundClass* MusicSoundClass;

	UPROPERTY(EditDefaultsOnly, Category = "Audio")
	class USoundClass* SFXSoundClass;

protected:
	virtual void NativeConstruct() override;

	// UI Bindings
	UPROPERTY(meta = (BindWidget))
	UButton* Btn_Play;

	UPROPERTY(meta = (BindWidget))
	UButton* Btn_Settings;

	UPROPERTY(meta = (BindWidget))
	UButton* Btn_Quit;

	UPROPERTY(meta = (BindWidget))
	UButton* Btn_CloseSettings;

	UPROPERTY(meta = (BindWidget))
	UWidgetSwitcher* MenuSwitcher; // Switches between Main Menu and Settings

	UPROPERTY(meta = (BindWidget))
	USlider* Slider_Music;

	UPROPERTY(meta = (BindWidget))
	USlider* Slider_SFX;

	// Button Functions
	UFUNCTION() void OnPlayClicked();
	UFUNCTION() void OnSettingsClicked();
	UFUNCTION() void OnQuitClicked();
	UFUNCTION() void OnCloseSettingsClicked();

	// Slider Functions
	UFUNCTION() void OnMusicVolumeChanged(float Value);
	UFUNCTION() void OnSFXVolumeChanged(float Value);
};