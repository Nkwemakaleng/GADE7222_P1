#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTService_UpdateBotState.h"
#include "BTTask_MoveToPickup.generated.h"

class ABaseCharacter;
class UBlackboardComponent;
class APickupBase;

UCLASS()
class GADE7222_P1_API UBTTask_MoveToPickup : public UBTTaskNode
{
	GENERATED_BODY()

public:
	UBTTask_MoveToPickup();

	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	virtual void TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;

	// Which Blackboard key holds the pickup ("HealthPickup" or "AmmoPickup")
	UPROPERTY(EditAnywhere, Category = "Pickup")
	FName PickupKeyName = TEXT("HealthPickup");

	// Distance at which we consider the pickup reached
	UPROPERTY(EditAnywhere, Category = "Pickup")
	float PickupRange = 200.f;

	// Timeout — safety so we never get stuck forever
	UPROPERTY(EditAnywhere, Category = "Pickup")
	float MaxMoveTime = 8.0f;

private:
	float AccumulatedTime = 0.0f;
	float StuckTime = 0.0f;
};