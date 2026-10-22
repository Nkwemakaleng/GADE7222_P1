#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BBKeys.h"
#include "BTTask_Flee.generated.h"

UCLASS()
class GADE7222_P1_API UBTTask_Flee : public UBTTaskNode
{
	GENERATED_BODY()

public:
	UBTTask_Flee();

	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	virtual void TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;

	UPROPERTY(EditAnywhere, Category = "Bot|Flee")
	float FleeDistance = 2000.f;

	UPROPERTY(EditAnywhere, Category = "Bot|Flee")
	float MaxFleeTime = 5.0f;

private:
	FVector FleeTargetLocation;
	float AccumulatedTime = 0.0f;
};