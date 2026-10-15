#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTService.h"
#include "BBKeys.h"
#include "PickupBase.h"
#include "BTService_UpdateBotState.generated.h"

UCLASS()
class GADE7222_P1_API UBTService_UpdateBotState : public UBTService
{
	GENERATED_BODY()

public:
	UBTService_UpdateBotState();

	// --- Perception ---
	UPROPERTY(EditAnywhere, Category = "Bot")
	float SightRange = 2000.0f;

	// Vertical offset for the eye position used by the LOS trace
	UPROPERTY(EditAnywhere, Category = "Bot")
	float EyeHeightOffset = 60.0f;

	// --- Health thresholds ---
	UPROPERTY(EditAnywhere, Category = "Bot")
	float FleeHealthThreshold = 25.0f;

	UPROPERTY(EditAnywhere, Category = "Bot")
	float SeekHealthThreshold = 75.0f;

protected:
	virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;

private:
	// Finds the nearest other ABaseCharacter that the bot can actually see (LOS check)
	AActor* FindNearestVisibleTarget(class ABaseCharacter* Bot) const;

	//Updated to find pickups by category 
	AActor* FindNearestPickupByCategory(class ABaseCharacter* Bot, EPickupCategory Category) const;

	// Does a line trace and returns true if clear
	bool HasLineOfSight(class ABaseCharacter* Bot, AActor* Target) const;

	// Decides the bot's intent and writes it to the Blackboard
	void UpdateIntent(class ABaseCharacter* Bot, class UBlackboardComponent* BB);
};