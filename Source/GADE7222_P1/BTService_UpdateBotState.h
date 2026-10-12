#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTService.h"
#include "BBKeys.h"
#include "BTService_UpdateBotState.generated.h"

UCLASS()
class GADE7222_P1_API UBTService_UpdateBotState : public UBTService
{
	GENERATED_BODY()

public:
	UBTService_UpdateBotState();

	// Perception filter
	UPROPERTY(EditAnywhere, Category = "Bot")
	float SightRange = 2000.0f;

	UPROPERTY(EditAnywhere, Category = "Bot")
	float FleeHealthThreshold = 25.0f;

	UPROPERTY(EditAnywhere, Category = "Bot")
	float SeekHealthThreshold = 75.0f;

protected:
	virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;

private:
	AActor* FindNearestTarget(class ABaseCharacter* Bot) const;
	AActor* FindNearestPickupByTag(class ABaseCharacter* Bot, const FString& TagFragment) const;
	void    UpdateIntent(class ABaseCharacter* Bot, class UBlackboardComponent* BB);
};