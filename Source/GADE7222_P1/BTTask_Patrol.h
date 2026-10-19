#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BBKeys.h"
#include "BTTask_Patrol.generated.h"

class ABaseCharacter;
class UBlackboardComponent;

UCLASS()
class GADE7222_P1_API UBTTask_Patrol : public UBTTaskNode
{
	GENERATED_BODY()

public:
	UBTTask_Patrol();

	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	virtual void TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;

	UPROPERTY(EditAnywhere, Category = "Patrol")
	float ArrivalRadius = 150.f;

	UPROPERTY(EditAnywhere, Category = "Patrol")
	float PatrolSearchRadius = 2000.f;

	UPROPERTY(EditAnywhere, Category = "Patrol")
	float MaxPatrolTime = 8.0f;
	
	UPROPERTY(EditAnywhere, Category = "Patrol")
	float TurnInterpSpeed = 6.0f;

private:
	float AccumulatedTime = 0.0f;
	float StuckTime = 0.0f;
	void PickRandomPatrolPoint(ABaseCharacter* Bot, UBlackboardComponent* BB, UWorld* World);
};