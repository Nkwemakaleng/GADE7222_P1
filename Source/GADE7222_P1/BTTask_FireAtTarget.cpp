#include "BTTask_FireAtTarget.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "AIController.h"
#include "BaseCharacter.h"
#include "DamageableInterface.h"
#include "CombatComponent.h"

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

	// --- Pick the best weapon for combat right now ---
	if (Bot->CombatComponent)
	{
		Bot->CombatComponent->SwitchToBestCombatWeapon();
	}
	// Dash toward the target if we're far away, to close the gap fast
if (Bot->CombatComponent)
{
	UCombatComponent* CC = Bot->CombatComponent;
	if (CC->FindBestCombatWeaponIndex() != INDEX_NONE)
	{
		if (Target && Bot->CanDash())
		{
			const FVector BotLoc = Bot->GetActorLocation();
			const FVector TargetLoc = Target->GetActorLocation();
			const float Dist = FVector::Dist(BotLoc, TargetLoc);

			if (Dist > 1200.f)
			{
				FVector Dir = (TargetLoc - BotLoc);
				Dir.Z = 0.f;
				Bot->TryDash(Dir.GetSafeNormal());
			}
		}
	}
}
	AccumulatedTime = 0.f;
	AI->SetFocalPoint(Target->GetActorLocation());
	Bot->StartFire();
	return EBTNodeResult::InProgress;
}

void UBTTask_FireAtTarget::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	AAIController* AI = OwnerComp.GetAIOwner();
	ABaseCharacter* Bot = Cast<ABaseCharacter>(AI ? AI->GetPawn() : nullptr);
	AActor* Target = GetTarget(OwnerComp);
	UBlackboardComponent* BB = OwnerComp.GetBlackboardComponent();

	if (!Bot || !AI || !BB)
	{
		if (Bot) Bot->StopFire();
		FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
		return;
	}

	// --- Target gone (dead or cleared from BB) ---
	if (!Target)
	{
		Bot->StopFire();
		FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
		return;
	}

	// --- Target died ---
	if (Target->GetClass()->ImplementsInterface(UDamageableInterface::StaticClass()))
	{
		if (!IDamageableInterface::Execute_IsAlive(Target))
		{
			Bot->StopFire();
			FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
			return;
		}
	}

	const bool bHasLOS = BB->GetValueAsBool(BotBBKeys::HasLineOfSight);

	const FVector BotLoc = Bot->GetActorLocation();
	const FVector TargetLoc = Target->GetActorLocation();
	const float Dist = FVector::Dist(BotLoc, TargetLoc);

	// --- Face target regardless of LOS (we may still be chasing) ---
	FVector ToTarget = TargetLoc - BotLoc;
	ToTarget.Z = 0.f;
	const FRotator DesiredRot = FRotator(0.f, ToTarget.Rotation().Yaw, 0.f);
	const FRotator NewControlRot = FMath::RInterpTo(AI->GetControlRotation(), DesiredRot, DeltaSeconds, 8.0f);
	AI->SetControlRotation(NewControlRot);

	// --- Movement: always keep chasing until we're in the sweet spot ---
	if (Dist > MaxFireDistance)
	{
		Bot->MoveForward(1.0f);
	}
	else if (Dist < MinFireDistance)
	{
		Bot->MoveForward(-1.0f);
	}
	else
	{
		// In range — only strafe if we can actually see the target
		if (bHasLOS)
		{
			Bot->MoveRight(FMath::Sin(GetWorld()->TimeSeconds * 2.5f));
		}
	}

	// --- Firing is gated on LOS, but the task KEEPS RUNNING either way ---
	if (bHasLOS && Dist < MaxFireDistance)
	{
		Bot->StartFire();
	}
	else
	{
		Bot->StopFire();
	}

	// --- Timeout: only exit when engagement has lasted too long ---
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