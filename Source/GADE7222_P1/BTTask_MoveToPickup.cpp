#include "BTTask_MoveToPickup.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "AIController.h"
#include "BaseCharacter.h"

UBTTask_MoveToPickup::UBTTask_MoveToPickup()
{
	NodeName = TEXT("Move To Pickup");
	bNotifyTick = true;
}

EBTNodeResult::Type UBTTask_MoveToPickup::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	UBlackboardComponent* BB = OwnerComp.GetBlackboardComponent();
	AAIController* AI = OwnerComp.GetAIOwner();
	if (!BB || !AI) return EBTNodeResult::Failed;

	AActor* Pickup = Cast<AActor>(BB->GetValueAsObject(PickupKeyName));
	if (!Pickup) return EBTNodeResult::Failed;

	AI->MoveToActor(Pickup, PickupRange * 0.5f);
	return EBTNodeResult::InProgress;
}

void UBTTask_MoveToPickup::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	UBlackboardComponent* BB = OwnerComp.GetBlackboardComponent();
	AAIController* AI = OwnerComp.GetAIOwner();
	ABaseCharacter* Bot = Cast<ABaseCharacter>(AI ? AI->GetPawn() : nullptr);
	if (!BB || !Bot || !AI)
	{
		FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
		return;
	}

	AActor* Pickup = Cast<AActor>(BB->GetValueAsObject(PickupKeyName));
	if (!Pickup)
	{
		FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
		return;
	}

	if (FVector::Dist(Bot->GetActorLocation(), Pickup->GetActorLocation()) <= PickupRange)
	{
		Bot->TryInteract();
		FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
	}
}