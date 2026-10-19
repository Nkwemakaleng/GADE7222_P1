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

	AccumulatedTime = 0.f;
	PickRandomPatrolPoint(Bot, BB, GetWorld());

	const FVector Target = BB->GetValueAsVector(BotBBKeys::PatrolPoint);
	UE_LOG(LogTemp, Warning, TEXT("Patrol: picked target %s"), *Target.ToString());

	return EBTNodeResult::InProgress;
}

void UBTTask_Patrol::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	AAIController* AI = OwnerComp.GetAIOwner();
	UBlackboardComponent* BB = OwnerComp.GetBlackboardComponent();
	ABaseCharacter* Bot = Cast<ABaseCharacter>(AI ? AI->GetPawn() : nullptr);
	if (!AI || !BB || !Bot) { FinishLatentTask(OwnerComp, EBTNodeResult::Failed); return; }

	const FVector Target = BB->GetValueAsVector(BotBBKeys::PatrolPoint);
	const FVector BotLoc = Bot->GetActorLocation();

	FVector ToTarget = Target - BotLoc;
	ToTarget.Z = 0.f;
	const float Dist = ToTarget.Size();

	// --- Arrived ---
	if (Dist < ArrivalRadius)
	{
		FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
		return;
	}

	// --- Smoothly turn control rotation toward the target ---
	const FRotator CurrentControlRot = AI->GetControlRotation();
	const FRotator DesiredRot = FRotator(0.f, ToTarget.Rotation().Yaw, 0.f);
	const FRotator NewControlRot = FMath::RInterpTo(CurrentControlRot, DesiredRot, DeltaSeconds, TurnInterpSpeed);
	AI->SetControlRotation(NewControlRot);

	// --- Move forward using the same function the player uses ---
	Bot->MoveForward(1.0f);

	// --- Stuck check ---
	if (Bot->GetVelocity().SizeSquared2D() < 100.f)
	{
		StuckTime += DeltaSeconds;
		if (StuckTime > 2.0f)
		{
			// We are stuck against geometry, pick a new point
			FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
			return;
		}
	}
	else
	{
		StuckTime = 0.f;
	}

	// --- Timeout safety ---
	AccumulatedTime += DeltaSeconds;
	if (AccumulatedTime > MaxPatrolTime)
	{
		FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
	}
}

void UBTTask_Patrol::PickRandomPatrolPoint(ABaseCharacter* Bot, UBlackboardComponent* BB, UWorld* World)
{
	if (!Bot || !BB) return;

	const FVector Origin = Bot->GetActorLocation();

	// Try navmesh first for a valid reachable point
	if (World)
	{
		UNavigationSystemV1* Nav = FNavigationSystem::GetCurrent<UNavigationSystemV1>(World);
		if (Nav)
		{
			FNavLocation Result;
			if (Nav->GetRandomReachablePointInRadius(Origin, PatrolSearchRadius, Result))
			{
				BB->SetValueAsVector(BotBBKeys::PatrolPoint, Result.Location);
				return;
			}
		}
	}

	// Fallback: raw direction if navmesh unavailable
	const FVector RandDir = FMath::VRand();
	const FVector RandPoint = Origin + FVector(RandDir.X, RandDir.Y, 0.f) * FMath::FRandRange(500.f, PatrolSearchRadius);
	BB->SetValueAsVector(BotBBKeys::PatrolPoint, RandPoint);
}