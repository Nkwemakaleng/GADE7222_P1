#include "BTTask_MoveToPickup.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "AIController.h"
#include "BaseCharacter.h"
#include "PickupBase.h"

UBTTask_MoveToPickup::UBTTask_MoveToPickup()
{
	NodeName = TEXT("Move To Pickup");
	bNotifyTick = true;
}

EBTNodeResult::Type UBTTask_MoveToPickup::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AAIController* AI = OwnerComp.GetAIOwner();
	UBlackboardComponent* BB = OwnerComp.GetBlackboardComponent();
	ABaseCharacter* Bot = Cast<ABaseCharacter>(AI ? AI->GetPawn() : nullptr);
	if (!AI || !BB || !Bot) return EBTNodeResult::Failed;

	AActor* Pickup = Cast<AActor>(BB->GetValueAsObject(PickupKeyName));
	if (!Pickup) return EBTNodeResult::Failed;

	AccumulatedTime = 0.0f;
	UE_LOG(LogTemp, Warning, TEXT("[MoveToPickup] Entered. KeyName=%s Pickup=%s"),
	*PickupKeyName.ToString(),
	Pickup ? *Pickup->GetName() : TEXT("NULL"));
	return EBTNodeResult::InProgress;
}

void UBTTask_MoveToPickup::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	AAIController* AI = OwnerComp.GetAIOwner();
	UBlackboardComponent* BB = OwnerComp.GetBlackboardComponent();
	ABaseCharacter* Bot = Cast<ABaseCharacter>(AI ? AI->GetPawn() : nullptr);
	if (!AI || !BB || !Bot)
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

	const FVector BotLoc = Bot->GetActorLocation();
	const FVector PickupLoc = Pickup->GetActorLocation();

	FVector ToPickup = PickupLoc - BotLoc;
	ToPickup.Z = 0.f;
	const float Dist = ToPickup.Size();

	// --- Arrived: try to consume ---
	if (Dist <= PickupRange)
	{
		if (APickupBase* PB = Cast<APickupBase>(Pickup))
		{
			// Direct consume — no reliance on camera raycast
			IInteractableInterface::Execute_Interact(Pickup, Bot);
		}
		FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
		return;
	}

	// --- Face the pickup using control rotation ---
	const FRotator DesiredRot = FRotator(0.f, ToPickup.Rotation().Yaw, 0.f);
	const FRotator NewControlRot = FMath::RInterpTo(AI->GetControlRotation(), DesiredRot, DeltaSeconds, 8.0f);
	AI->SetControlRotation(NewControlRot);

	// --- Move forward (same function the player uses) ---
	Bot->MoveForward(1.0f);

	// --- Timeout so we never hang ---
	AccumulatedTime += DeltaSeconds;
	if (AccumulatedTime > MaxMoveTime)
	{
		UE_LOG(LogTemp, Warning, TEXT("MoveToPickup: timed out after %.1fs (dist to pickup %.1f)"),
			MaxMoveTime, Dist);
		FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
	}
}