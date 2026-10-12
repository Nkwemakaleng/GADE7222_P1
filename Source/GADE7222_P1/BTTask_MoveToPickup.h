#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTService_UpdateBotState.h"
#include "BTTask_MoveToPickup.generated.h"

UCLASS()
class GADE7222_P1_API UBTTask_MoveToPickup : public UBTTaskNode
{
	GENERATED_BODY()

public:
	UBTTask_MoveToPickup();

	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	virtual void TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;

	// "HealthPickup" or "AmmoPickup"
	UPROPERTY(EditAnywhere, Category = "Bot")
	FName PickupKeyName = TEXT("HealthPickup");

	UPROPERTY(EditAnywhere, Category = "Bot")
	float PickupRange = 150.f;
};