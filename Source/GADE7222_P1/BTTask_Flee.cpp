#include "BTTask_Flee.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "AIController.h"
#include "BaseCharacter.h"
#include "NavigationSystem.h"
#include "StaminaComponent.h"

UBTTask_Flee::UBTTask_Flee()
{
	NodeName = TEXT("Flee");
	bNotifyTick = true;
}

EBTNodeResult::Type UBTTask_Flee::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AAIController* AI = OwnerComp.GetAIOwner();
	UBlackboardComponent* BB = OwnerComp.GetBlackboardComponent();
	ABaseCharacter* Bot = Cast<ABaseCharacter>(AI ? AI->GetPawn() : nullptr);
	if (!AI || !BB || !Bot) return EBTNodeResult::Failed;

	// Stop shooting when running away
	Bot->StopFire();
	AccumulatedTime = 0.0f;

	// Find threat (TargetActor or LastAttacker)
	AActor* Target = Cast<AActor>(BB->GetValueAsObject(BotBBKeys::TargetActor));
	if (!Target)
	{
		// If no target, just run in a random direction
		FVector RandDir = FMath::VRand();
		FleeTargetLocation = Bot->GetActorLocation() + (RandDir * FleeDistance);
	}
	else
	{
		// Run directly away from the threat
		FVector DirAway = (Bot->GetActorLocation() - Target->GetActorLocation()).GetSafeNormal();
		FleeTargetLocation = Bot->GetActorLocation() + (DirAway * FleeDistance);
	}

	// Ensure the point is on the NavMesh
	UNavigationSystemV1* Nav = FNavigationSystem::GetCurrent<UNavigationSystemV1>(GetWorld());
	if (Nav)
	{
		FNavLocation Result;
		if (Nav->GetRandomReachablePointInRadius(FleeTargetLocation, 500.f, Result))
		{
			FleeTargetLocation = Result.Location;
		}
	}

	AI->MoveToLocation(FleeTargetLocation, 100.f);
	return EBTNodeResult::InProgress;
}

void UBTTask_Flee::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	AAIController* AI = OwnerComp.GetAIOwner();
	ABaseCharacter* Bot = Cast<ABaseCharacter>(AI ? AI->GetPawn() : nullptr);
	if (!AI || !Bot) { FinishLatentTask(OwnerComp, EBTNodeResult::Failed); return; }

	// Keep the bot sprinting if it has stamina
	if (Bot->StaminaComponent && Bot->StaminaComponent->HasStamina())
	{
		Bot->StartSprint();
	}

	// Succeed if we reached the point or ran out of time
	float DistToFleePoint = FVector::Dist(Bot->GetActorLocation(), FleeTargetLocation);
	AccumulatedTime += DeltaSeconds;

	if (DistToFleePoint < 200.f || AccumulatedTime > MaxFleeTime)
	{
		FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
	}
}