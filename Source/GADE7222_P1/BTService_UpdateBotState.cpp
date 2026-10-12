#include "BTService_UpdateBotState.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "AIController.h"
#include "BaseCharacter.h"
#include "HealthComponent.h"
#include "CombatComponent.h"
#include "PickupBase.h"
#include "Kismet/GameplayStatics.h"

UBTService_UpdateBotState::UBTService_UpdateBotState()
{
	NodeName = TEXT("Update Bot State");
	Interval = 0.25f;
	RandomDeviation = 0.05f;
	bNotifyTick = true;
}

void UBTService_UpdateBotState::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

	AAIController* AI = OwnerComp.GetAIOwner();
	UBlackboardComponent* BB = OwnerComp.GetBlackboardComponent();
	if (!AI || !BB) return;

	ABaseCharacter* Bot = Cast<ABaseCharacter>(AI->GetPawn());
	if (!Bot) return;

	// --- Target ---
	AActor* Target = FindNearestTarget(Bot);
	BB->SetValueAsObject(BotBBKeys::TargetActor, Target);
	BB->SetValueAsBool(BotBBKeys::HasLineOfSight, Target != nullptr);

	// --- Nearest pickups (tag-based; see editor note below) ---
	BB->SetValueAsObject(BotBBKeys::HealthPickup, FindNearestPickupByTag(Bot, TEXT("Health")));
	BB->SetValueAsObject(BotBBKeys::AmmoPickup,   FindNearestPickupByTag(Bot, TEXT("Ammo")));

	// --- Intent ---
	UpdateIntent(Bot, BB);

	// --- Patrol point ---
	if (BB->GetValueAsVector(BotBBKeys::PatrolPoint).IsNearlyZero())
	{
		// Seed a random point around the bot
		const FVector Origin = Bot->GetActorLocation();
		const FVector Rand = FMath::VRand() * FMath::FRandRange(500.f, 1500.f);
		BB->SetValueAsVector(BotBBKeys::PatrolPoint, Origin + FVector(Rand.X, Rand.Y, 0.f));
	}
}

AActor* UBTService_UpdateBotState::FindNearestTarget(ABaseCharacter* Bot) const
{
	if (!Bot) return nullptr;

	TArray<AActor*> AllChars;
	UGameplayStatics::GetAllActorsOfClass(Bot->GetWorld(), ABaseCharacter::StaticClass(), AllChars);

	AActor* Best = nullptr;
	float BestDistSq = SightRange * SightRange;

	for (AActor* A : AllChars)
	{
		if (!A || A == Bot) continue;
		const float D = FVector::DistSquared(Bot->GetActorLocation(), A->GetActorLocation());
		if (D < BestDistSq)
		{
			BestDistSq = D;
			Best = A;
		}
	}
	return Best;
}

AActor* UBTService_UpdateBotState::FindNearestPickupByTag(ABaseCharacter* Bot, const FString& TagFragment) const
{
	if (!Bot) return nullptr;

	TArray<AActor*> AllPickups;
	UGameplayStatics::GetAllActorsOfClass(Bot->GetWorld(), APickupBase::StaticClass(), AllPickups);

	AActor* Best = nullptr;
	float BestDistSq = TNumericLimits<float>::Max();

	for (AActor* A : AllPickups)
	{
		if (!A) continue;
		if (!A->GetClass()->GetName().Contains(TagFragment)) continue;

		const float D = FVector::DistSquared(Bot->GetActorLocation(), A->GetActorLocation());
		if (D < BestDistSq) { BestDistSq = D; Best = A; }
	}
	return Best;
}

void UBTService_UpdateBotState::UpdateIntent(ABaseCharacter* Bot, UBlackboardComponent* BB)
{
	if (!Bot || !Bot->HealthComponent || !Bot->CombatComponent) return;

	const float HP = Bot->HealthComponent->CurrentHealth;
	AActor* Target = Cast<AActor>(BB->GetValueAsObject(BotBBKeys::TargetActor));
	AActor* HealthP = Cast<AActor>(BB->GetValueAsObject(BotBBKeys::HealthPickup));
	AActor* AmmoP   = Cast<AActor>(BB->GetValueAsObject(BotBBKeys::AmmoPickup));

	EBotIntent Intent = EBotIntent::Patrol;

	// Priority 1: Retreat if low HP
	if (HP < FleeHealthThreshold)
	{
		Intent = HealthP ? EBotIntent::SeekHealth : EBotIntent::Flee;
	}
	else if (Target)
	{
		// Priority 2: If engage target exists
		UCombatComponent* CC = Bot->CombatComponent;
		const bool bLowAmmo = CC->GetCurrentAmmo() < CC->GetMaxAmmo() / 2;
		Intent = (bLowAmmo && AmmoP) ? EBotIntent::SeekAmmo : EBotIntent::Engage;
	}
	else if (HP < SeekHealthThreshold && HealthP)
	{
		// Priority 3: proactively seek health between engagements
		Intent = EBotIntent::SeekHealth;
	}

	BB->SetValueAsEnum(BotBBKeys::CurrentIntent, static_cast<uint8>(Intent));
}