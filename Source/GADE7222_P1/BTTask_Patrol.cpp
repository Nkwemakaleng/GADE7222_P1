#include "BTTask_Patrol.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "AIController.h"
#include "BaseCharacter.h"
#include "NavigationSystem.h"

UBTTask_Patrol::UBTTask_Patrol()
{
	NodeName = TEXT("Patrol");
	bNotifyTick = true;
}

EBTNodeResult::Type UBTTask_Patrol::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AAIController* AI = OwnerComp.GetAIOwner();
	UBlackboardComponent* BB = OwnerComp.GetBlackboardComponent();
	ABaseCharacter* Bot = Cast<ABaseCharacter>(AI ? AI->GetPawn() : nullptr);
	if (!AI || !BB || !Bot) return EBTNodeResult::Failed;

	FVector Target = BB->GetValueAsVector(BotBBKeys::PatrolPoint);
	if (Target.IsNearlyZero())
	{
		Target = Bot->GetActorLocation();
	}

	AI->MoveToLocation(Target, ArrivalRadius);
	return EBTNodeResult::InProgress;
}

void UBTTask_Patrol::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	AAIController* AI = OwnerComp.GetAIOwner();
	UBlackboardComponent* BB = OwnerComp.GetBlackboardComponent();
	ABaseCharacter* Bot = Cast<ABaseCharacter>(AI ? AI->GetPawn() : nullptr);
	if (!AI || !BB || !Bot) { FinishLatentTask(OwnerComp, EBTNodeResult::Failed); return; }

	const FVector Target = BB->GetValueAsVector(BotBBKeys::PatrolPoint);

	if (FVector::Dist(Bot->GetActorLocation(), Target) < ArrivalRadius)
	{
		// Pick a new patrol point
		UNavigationSystemV1* Nav = FNavigationSystem::GetCurrent<UNavigationSystemV1>(GetWorld());
		if (Nav)
		{
			FNavLocation Result;
			if (Nav->GetRandomReachablePointInRadius(Bot->GetActorLocation(), PatrolSearchRadius, Result))
			{
				BB->SetValueAsVector(BotBBKeys::PatrolPoint, Result.Location);
				AI->MoveToLocation(Result.Location, ArrivalRadius);
			}
		}
	}
}