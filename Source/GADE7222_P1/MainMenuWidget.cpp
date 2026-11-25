#include "MainMenuWidget.h"
#include "Components/Button.h"
#include "Components/Slider.h"
#include "Components/WidgetSwitcher.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Sound/SoundMix.h"
#include "Sound/SoundClass.h"
#include "MyPlayerController.h"

void UMainMenuWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (Btn_Play) Btn_Play->OnClicked.AddDynamic(this, &UMainMenuWidget::OnPlayClicked);
	if (Btn_Settings) Btn_Settings->OnClicked.AddDynamic(this, &UMainMenuWidget::OnSettingsClicked);
	if (Btn_Quit) Btn_Quit->OnClicked.AddDynamic(this, &UMainMenuWidget::OnQuitClicked);
	if (Btn_CloseSettings) Btn_CloseSettings->OnClicked.AddDynamic(this, &UMainMenuWidget::OnCloseSettingsClicked);

	if (Slider_Music) Slider_Music->OnValueChanged.AddDynamic(this, &UMainMenuWidget::OnMusicVolumeChanged);
	if (Slider_SFX) Slider_SFX->OnValueChanged.AddDynamic(this, &UMainMenuWidget::OnSFXVolumeChanged);

	// Show mouse cursor and set UI mode
	
	if (AMyPlayerController* PC = Cast<AMyPlayerController>(GetOwningPlayer()))
	{
		PC->SetGameFlowState(EGameFlowState::TitleScreen);
	}
}

void UMainMenuWidget::OnPlayClicked()
{
	// Load the gameplay map
	UGameplayStatics::OpenLevel(this, FName("TestMap")); 
}

void UMainMenuWidget::OnSettingsClicked()
{
	if (MenuSwitcher) MenuSwitcher->SetActiveWidgetIndex(1); // Assumes index 1 is Settings
}

void UMainMenuWidget::OnCloseSettingsClicked()
{
	if (MenuSwitcher) MenuSwitcher->SetActiveWidgetIndex(0); // Assumes index 0 is Main Menu
}

void UMainMenuWidget::OnQuitClicked()
{
	UKismetSystemLibrary::QuitGame(this, GetOwningPlayer(), EQuitPreference::Quit, false);
}

void UMainMenuWidget::OnMusicVolumeChanged(float Value)
{
	if (MasterSoundMix && MusicSoundClass)
	{
		// Value comes in as 1.0 to 0.0
		UGameplayStatics::SetSoundMixClassOverride(this, MasterSoundMix, MusicSoundClass, Value, 1.0f, 0.0f);
		UGameplayStatics::PushSoundMixModifier(this, MasterSoundMix);
	}
}

void UMainMenuWidget::OnSFXVolumeChanged(float Value)
{
	if (MasterSoundMix && SFXSoundClass)
	{
		UGameplayStatics::SetSoundMixClassOverride(this, MasterSoundMix, SFXSoundClass, Value, 1.0f, 0.0f);
		UGameplayStatics::PushSoundMixModifier(this, MasterSoundMix);
	}
}