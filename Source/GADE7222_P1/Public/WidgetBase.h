// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Runtime/UMG/Public/Blueprint/UserWidget.h"
#include "WidgetBase.generated.h"

/**
 * 
 */
UCLASS()
class GADE7222_P1_API UWidgetBase : public UUserWidget
{
	
public:
#if WITH_EDITOR
	virtual const FText GetPaletteCategory() override;
#endif
private:
	GENERATED_BODY()
};
