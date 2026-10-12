#pragma once

#include "CoreMinimal.h"
#include "BaseCharacter.h"
#include "EnemyCharacter.generated.h"

UCLASS()
class GADE7222_P1_API AEnemyCharacter : public ABaseCharacter
{
	GENERATED_BODY()

public:
	AEnemyCharacter();

	// Distinguishable appearance — set per-instance in the level
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Bot Visual")
	FLinearColor BotColor = FLinearColor::Red;

protected:
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, Category = "Bot Visual")
	UMaterialInstanceDynamic* BodyMID;

	void ApplyBotColor();
};