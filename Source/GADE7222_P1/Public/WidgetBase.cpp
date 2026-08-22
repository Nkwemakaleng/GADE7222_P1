// Fill out your copyright notice in the Description page of Project Settings.
#include "WidgetBase.h"
#include "GADE7222_P1\Public\WidgetBase.h"
#include "CoreMinimal.h"

#if WITH_EDITOR
const FText UWidgetBase::GetPaletteCategory()
{
	return NSLOCTEXT("UMG", "CustomPaletteCategory", "GameDev!");
}
#endif	