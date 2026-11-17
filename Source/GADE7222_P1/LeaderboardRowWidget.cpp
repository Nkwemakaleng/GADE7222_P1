#include "LeaderboardRowWidget.h"
#include "Components/TextBlock.h"

void ULeaderboardRowWidget::SetRowData(int32 Position, const FString& Name, int32 Kills, int32 Deaths, bool bIsLocalPlayer)
{
	if (Text_Position) Text_Position->SetText(FText::AsNumber(Position));
	if (Text_Name) Text_Name->SetText(FText::FromString(Name));
	if (Text_Kills) Text_Kills->SetText(FText::AsNumber(Kills));
	if (Text_Deaths) Text_Deaths->SetText(FText::AsNumber(Deaths));

	// Highlight the player
	FLinearColor RowColor = bIsLocalPlayer ? FLinearColor(1.0f, 0.8f, 0.0f) : FLinearColor::White;
	
	if (Text_Position) Text_Position->SetColorAndOpacity(RowColor);
	if (Text_Name) Text_Name->SetColorAndOpacity(RowColor);
	if (Text_Kills) Text_Kills->SetColorAndOpacity(RowColor);
	if (Text_Deaths) Text_Deaths->SetColorAndOpacity(RowColor);
}