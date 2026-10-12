#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTService_UpdateBotState.h"
#include "BTTask_Patrol.generated.h"

UCLASS()
class GADE7222_P1_API UBTTask_Patrol : public UBTTaskNode
{
	GENERATED_BODY()

public:
	UBTTask_Patrol();

	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	virtual void TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;

	UPROPERTY(EditAnywhere, Category = "Bot")
	float ArrivalRadius = 150.f;

	UPROPERTY(EditAnywhere, Category = "Bot")
	float PatrolSearchRadius = 1500.f;
};