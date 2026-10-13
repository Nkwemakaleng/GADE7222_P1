#include "BTTask_FireAtTarget.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "AIController.h"
#include "BaseCharacter.h"
#include "DamageableInterface.h"

UBTTask_FireAtTarget::UBTTask_FireAtTarget()
{
	NodeName = TEXT("Fire At Target");
	bNotifyTick = true;
}

EBTNodeResult::Type UBTTask_FireAtTarget::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AAIController* AI = OwnerComp.GetAIOwner();
	ABaseCharacter* Bot = Cast<ABaseCharacter>(AI ? AI->GetPawn() : nullptr);
	if (!Bot || !AI) return EBTNodeResult::Failed;

	AActor* Target = GetTarget(OwnerComp);
	if (!Target) return EBTNodeResult::Failed;

	AccumulatedTime = 0.f;
	Bot->StartFire();
	return EBTNodeResult::InProgress;
}

void UBTTask_FireAtTarget::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	AAIController* AI = OwnerComp.GetAIOwner();
	ABaseCharacter* Bot = Cast<ABaseCharacter>(AI ? AI->GetPawn() : nullptr);
	AActor* Target = GetTarget(OwnerComp);

	if (!Bot || !Target || !AI)
	{
		if (Bot) Bot->StopFire();
		FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
		return;
	}

	const FVector BotLoc = Bot->GetActorLocation();
	const FVector TargetLoc = Target->GetActorLocation();
	const float Dist = FVector::Dist(BotLoc, TargetLoc);

	// --- Face target (control rotation, same as mouse) ---
	FVector ToTarget = TargetLoc - BotLoc;
	ToTarget.Z = 0.f;
	const FRotator DesiredRot = FRotator(0.f, ToTarget.Rotation().Yaw, 0.f);
	const FRotator NewControlRot = FMath::RInterpTo(AI->GetControlRotation(), DesiredRot, DeltaSeconds, 8.0f);
	AI->SetControlRotation(NewControlRot);

	// --- Movement using the player's functions ---
	const FVector ForwardDir = NewControlRot.Vector();

	if (Dist > MaxFireDistance)
	{
		// Close the gap
		Bot->MoveForward(1.0f);
	}
	else if (Dist < MinFireDistance)
	{
		// Back off
		Bot->MoveForward(-1.0f);
	}
	else
	{
		// Strafe side to side using MoveRight
		const float StrafeAmount = FMath::Sin(GetWorld()->TimeSeconds * 2.5f);
		Bot->MoveRight(StrafeAmount);
	}

	// --- Safety exits ---
	if (Target->GetClass()->ImplementsInterface(UDamageableInterface::StaticClass()))
	{
		if (!IDamageableInterface::Execute_IsAlive(Target))
		{
			Bot->StopFire();
			FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
			return;
		}
	}

	if (UBlackboardComponent* BB = OwnerComp.GetBlackboardComponent())
	{
		if (!BB->GetValueAsBool(BotBBKeys::HasLineOfSight))
		{
			Bot->StopFire();
			FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
			return;
		}
	}

	AccumulatedTime += DeltaSeconds;
	if (AccumulatedTime > MaxEngageDuration)
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