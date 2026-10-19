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
	
	UE_LOG(LogTemp, Warning, TEXT("[MoveToPickup] Entered. KeyName=%s Pickup=%s"), 
		*PickupKeyName.ToString(), Pickup ? *Pickup->GetName() : TEXT("NULL"));

	if (!Pickup) return EBTNodeResult::Failed;

	AccumulatedTime = 0.0f;
	StuckTime = 0.0f;
	return EBTNodeResult::InProgress;
}

void UBTTask_MoveToPickup::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	AAIController* AI = OwnerComp.GetAIOwner();
	UBlackboardComponent* BB = OwnerComp.GetBlackboardComponent();
	ABaseCharacter* Bot = Cast<ABaseCharacter>(AI ? AI->GetPawn() : nullptr);
	if (!AI || !BB || !Bot) { FinishLatentTask(OwnerComp, EBTNodeResult::Failed); return; }

	AActor* Pickup = Cast<AActor>(BB->GetValueAsObject(PickupKeyName));
	
	// If pickup was consumed or removed, SUCCEED (don't fail) so the tree re-evaluates smoothly
	if (!Pickup) 
	{
		FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
		return;
	}

	const FVector BotLoc = Bot->GetActorLocation();
	const FVector PickupLoc = Pickup->GetActorLocation();

	FVector ToPickup = PickupLoc - BotLoc;
	ToPickup.Z = 0.f;
	const float Dist = ToPickup.Size();

	// --- Arrived ---
	if (Dist <= PickupRange)
	{
		if (APickupBase* PB = Cast<APickupBase>(Pickup))
		{
			IInteractableInterface::Execute_Interact(Pickup, Bot);
		}
		FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
		return;
	}

	// --- Stuck check ---
	if (Bot->GetVelocity().SizeSquared2D() < 100.f)
	{
		StuckTime += DeltaSeconds;
		if (StuckTime > 2.0f)
		{
			UE_LOG(LogTemp, Warning, TEXT("[MoveToPickup] Stuck! Aborting task."));
			FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
			return;
		}
	}
	else
	{
		StuckTime = 0.f;
	}

	// --- Face and Move ---
	const FRotator DesiredRot = FRotator(0.f, ToPickup.Rotation().Yaw, 0.f);
	const FRotator NewControlRot = FMath::RInterpTo(AI->GetControlRotation(), DesiredRot, DeltaSeconds, 8.0f);
	AI->SetControlRotation(NewControlRot);

	Bot->MoveForward(1.0f);

	AccumulatedTime += DeltaSeconds;
	if (AccumulatedTime > MaxMoveTime)
	{
		FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
	}
}