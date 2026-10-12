#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTService_UpdateBotState.h"
#include "BTTask_FireAtTarget.generated.h"

UCLASS()
class GADE7222_P1_API UBTTask_FireAtTarget : public UBTTaskNode
{
	GENERATED_BODY()

public:
	UBTTask_FireAtTarget();

	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	virtual void TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;
	virtual EBTNodeResult::Type AbortTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

	UPROPERTY(EditAnywhere, Category = "Bot")
	float MinFireDistance = 200.f;

	UPROPERTY(EditAnywhere, Category = "Bot")
	float MaxFireDistance = 1800.f;

private:
	AActor* GetTarget(UBehaviorTreeComponent& OwnerComp) const;
};