#include "BTTask_FireAtTarget.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "AIController.h"
#include "BaseCharacter.h"
#include "DamageableInterface.h"
#include "CombatComponent.h"
#include "PickupBase.h"
#include "Engine/World.h"
#include "Kismet/GameplayStatics.h"

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

	AccumulatedTime = 0.0f;
	CurrentReactionTime = FMath::FRandRange(MinReactionTime, MaxReactionTime);
	bIsReadyToFire = false;
	StrafeDirection = FMath::RandBool() ? 1.0f : -1.0f;
	StrafeTimer = FMath::FRandRange(StrafeChangeMinTime, StrafeChangeMaxTime);

	if (Bot->CombatComponent)
	{
		float Dist = FVector::Dist(Bot->GetActorLocation(), Target->GetActorLocation());
		Bot->CombatComponent->SwitchToBestCombatWeapon(Dist);
	}

	AI->SetFocalPoint(Target->GetActorLocation());
	return EBTNodeResult::InProgress;
}

void UBTTask_FireAtTarget::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	AAIController* AI = OwnerComp.GetAIOwner();
	ABaseCharacter* Bot = Cast<ABaseCharacter>(AI ? AI->GetPawn() : nullptr);
	AActor* Target = GetTarget(OwnerComp);
	UBlackboardComponent* BB = OwnerComp.GetBlackboardComponent();

	if (!Bot || !AI || !BB || !Target)
	{
		if (Bot) Bot->StopFire();
		FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
		return;
	}

	// Exit if target dies or we lose sight
	if (Target->GetClass()->ImplementsInterface(UDamageableInterface::StaticClass()))
	{
		if (!IDamageableInterface::Execute_IsAlive(Target))
		{
			Bot->StopFire();
			FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
			return;
		}
	}

	bool bHasLOS = BB->GetValueAsBool(BotBBKeys::HasLineOfSight);
	if (!bHasLOS)
	{
		Bot->StopFire();
		FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
		return;
	}
	
	const FVector BotLoc = Bot->GetActorLocation();
	const FVector TargetLoc = Target->GetActorLocation();
	const float Dist = FVector::Dist(BotLoc, TargetLoc);
	
	// --- 1. FORCE ROTATION TOWARDS TARGET ---
	// Calculate the direction we need to look
	FVector ToTarget = TargetLoc - BotLoc;
	ToTarget.Z = 0.0f; // Keep it flat so we don't look at the sky/floor
	FRotator DesiredRot = ToTarget.Rotation();

	// Smoothly interpolate the Control Rotation
	FRotator CurrentControlRot = AI->GetControlRotation();
	FRotator NewControlRot = FMath::RInterpTo(CurrentControlRot, DesiredRot, DeltaSeconds, 8.0f);
	
	// Apply it to the controller
	AI->SetControlRotation(NewControlRot);
	
	// Force the Actor's rotation to match (bypasses any CMC conflicts)
	Bot->SetActorRotation(NewControlRot);
	// --- 2. REACTION TIME ---
	if (!bIsReadyToFire)
	{
		CurrentReactionTime -= DeltaSeconds;
		if (CurrentReactionTime <= 0.0f)
		{
			bIsReadyToFire = true;
			Bot->StartFire();
		}
		AI->SetFocalPoint(TargetLoc);
		return; 
	}

	// --- 3. AIM JITTER ---
	FVector AimLoc = TargetLoc;
	if (AimErrorDegrees > 0.0f)
	{
		FVector RandomOffset = FMath::VRand() * (AimErrorDegrees * 10.0f);
		AimLoc += RandomOffset;
	}
	AI->SetFocalPoint(AimLoc);

	// --- 4. MOVEMENT ---
	if (Dist > MaxFireDistance)
	{
		Bot->MoveForward(1.0f);
	}
	else if (Dist < MinFireDistance)
	{
		Bot->MoveForward(-1.0f);
		if (Bot->CanDash() && FMath::RandRange(0, 100) < 10)
		{
			FVector DodgeDir = (BotLoc - TargetLoc).GetSafeNormal();
			Bot->TryDash(DodgeDir);
		}
	}
	else
	{
		StrafeTimer -= DeltaSeconds;
		if (StrafeTimer <= 0.0f)
		{
			StrafeDirection = FMath::RandBool() ? 1.0f : -1.0f;
			StrafeTimer = FMath::FRandRange(StrafeChangeMinTime, StrafeChangeMaxTime);
		}
		Bot->MoveRight(StrafeDirection);

		if (FMath::FRand() < JumpChancePerTick)
		{
			Bot->Jump();
		}
	}

	// --- 5. TIMEOUT ---
	AccumulatedTime += DeltaSeconds;
	if (AccumulatedTime > MaxEngageDuration)
	{
		Bot->StopFire();
		FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
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