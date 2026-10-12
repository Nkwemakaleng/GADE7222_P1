#include "BTTask_FireAtTarget.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "AIController.h"
#include "BaseCharacter.h"

UBTTask_FireAtTarget::UBTTask_FireAtTarget()
{
	NodeName = TEXT("Fire At Target");
	bNotifyTick = true;
}

EBTNodeResult::Type UBTTask_FireAtTarget::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AAIController* AI = OwnerComp.GetAIOwner();
	ABaseCharacter* Bot = Cast<ABaseCharacter>(AI ? AI->GetPawn() : nullptr);
	if (!Bot) return EBTNodeResult::Failed;

	AActor* Target = GetTarget(OwnerComp);
	if (!Target) return EBTNodeResult::Failed;

	Bot->StartFire();
	return EBTNodeResult::InProgress;
}

void UBTTask_FireAtTarget::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	AAIController* AI = OwnerComp.GetAIOwner();
	ABaseCharacter* Bot = Cast<ABaseCharacter>(AI ? AI->GetPawn() : nullptr);
	AActor* Target = GetTarget(OwnerComp);

	if (!Bot || !Target)
	{
		if (Bot) Bot->StopFire();
		FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
		return;
	}

	const FVector BotLoc = Bot->GetActorLocation();
	const FVector TargetLoc = Target->GetActorLocation();
	const float Dist = FVector::Dist(BotLoc, TargetLoc);

	// Face the target
	const FRotator Look = (TargetLoc - BotLoc).Rotation();
	Bot->SetActorRotation(FRotator(0.f, Look.Yaw, 0.f));

	// Move into range / strafe if needed
	if (Dist > MaxFireDistance)
	{
		AI->MoveToActor(Target, MaxFireDistance * 0.8f);
	}
	else if (Dist < MinFireDistance)
	{
		// Back up
		const FVector Away = (BotLoc - TargetLoc).GetSafeNormal();
		AI->MoveToLocation(BotLoc + Away * 400.f);
	}
	else
	{
		// Strafe
		AI->StopMovement();
		const FVector Right = FRotationMatrix(FRotator(0.f, Look.Yaw, 0.f)).GetUnitAxis(EAxis::Y);
		Bot->AddMovementInput(Right, FMath::Sin(GetWorld()->TimeSeconds * 3.f));
	}

	// If target is dead, stop
	if (!IDamageableInterface::Execute_IsAlive(Target))
	{
		Bot->StopFire();
		FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
		return;
	}
}

EBTNodeResult::Type UBTTask_FireAtTarget::AbortTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AAIController* AI = OwnerComp.GetAIOwner();
	if (ABaseCharacter* Bot = Cast<ABaseCharacter>(AI ? AI->GetPawn() : nullptr))
	{
		Bot->StopFire();
	}
	return EBTNodeResult::Aborted;
}

AActor* UBTTask_FireAtTarget::GetTarget(UBehaviorTreeComponent& OwnerComp) const
{
	if (UBlackboardComponent* BB = OwnerComp.GetBlackboardComponent())
	{
		return Cast<AActor>(BB->GetValueAsObject(BotBBKeys::TargetActor));
	}
	return nullptr;
}