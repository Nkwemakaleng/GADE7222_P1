#include "BTService_UpdateBotState.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "AIController.h"
#include "BaseCharacter.h"
#include "HealthComponent.h"
#include "CombatComponent.h"
#include "PickupBase.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/World.h"

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

	// --- Find nearest target only if LOS is clear ---
	AActor* Target = FindNearestVisibleTarget(Bot);
	BB->SetValueAsObject(BotBBKeys::TargetActor, Target);
	BB->SetValueAsBool(BotBBKeys::HasLineOfSight, Target != nullptr);

	// --- Nearest pickups ---
	BB->SetValueAsObject(BotBBKeys::HealthPickup, FindNearestPickupByTag(Bot, TEXT("Health")));
	BB->SetValueAsObject(BotBBKeys::AmmoPickup,   FindNearestPickupByTag(Bot, TEXT("Ammo")));

	// --- Decide intent ---
	UpdateIntent(Bot, BB);
}

// ----------------------------------------------------------------
// LOS
// ----------------------------------------------------------------

bool UBTService_UpdateBotState::HasLineOfSight(ABaseCharacter* Bot, AActor* Target) const
{
	if (!Bot || !Target) return false;

	UWorld* World = Bot->GetWorld();
	if (!World) return false;

	const FVector Start = Bot->GetActorLocation() + FVector(0.f, 0.f, EyeHeightOffset);
	const FVector End   = Target->GetActorLocation() + FVector(0.f, 0.f, EyeHeightOffset);

	FHitResult Hit;
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(Bot);
	Params.AddIgnoredActor(Target);
	Params.bTraceComplex = false;

	const bool bHit = World->LineTraceSingleByChannel(Hit, Start, End, ECC_Visibility, Params);

	// If the trace hit nothing, LOS is clear
	return !bHit;
}

AActor* UBTService_UpdateBotState::FindNearestVisibleTarget(ABaseCharacter* Bot) const
{
	if (!Bot) return nullptr;

	TArray<AActor*> AllChars;
	UGameplayStatics::GetAllActorsOfClass(Bot->GetWorld(), ABaseCharacter::StaticClass(), AllChars);

	AActor* Best = nullptr;
	float BestDistSq = SightRange * SightRange;

	for (AActor* A : AllChars)
	{
		if (!A || A == Bot) continue;

		// Skip dead bots
		ABaseCharacter* AsChar = Cast<ABaseCharacter>(A);
		if (AsChar && AsChar->HealthComponent && AsChar->HealthComponent->IsDead()) continue;

		const float D = FVector::DistSquared(Bot->GetActorLocation(), A->GetActorLocation());
		if (D >= BestDistSq) continue;
		// Must be in front of the bot
		const FVector Facing = Bot->GetActorForwardVector();
		const FVector ToTarget = (A->GetActorLocation() - Bot->GetActorLocation()).GetSafeNormal();
		if (FVector::DotProduct(Facing, ToTarget) < 0.5f) continue; // ~60° half-angle
		// Must be visible
		if (!HasLineOfSight(Bot, A)) continue;

		BestDistSq = D;
		Best = A;
	}
	return Best;
}

// ----------------------------------------------------------------
// Pickups
// ----------------------------------------------------------------

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

// ----------------------------------------------------------------
// Intent
// ----------------------------------------------------------------

void UBTService_UpdateBotState::UpdateIntent(ABaseCharacter* Bot, UBlackboardComponent* BB)
{
	if (!Bot || !Bot->HealthComponent || !Bot->CombatComponent || !BB) return;

	const float HP = Bot->HealthComponent->CurrentHealth;
	AActor* Target  = Cast<AActor>(BB->GetValueAsObject(BotBBKeys::TargetActor));
	AActor* HealthP = Cast<AActor>(BB->GetValueAsObject(BotBBKeys::HealthPickup));
	AActor* AmmoP   = Cast<AActor>(BB->GetValueAsObject(BotBBKeys::AmmoPickup));

	EBotIntent Intent = EBotIntent::Patrol;

	// Priority 1: Retreat if low HP
	if (HP < FleeHealthThreshold)
	{
		Intent = HealthP ? EBotIntent::SeekHealth : EBotIntent::Flee;
	}
	// Priority 2: Engage if a visible target exists
	else if (Target)
	{
		UCombatComponent* CC = Bot->CombatComponent;
		const bool bLowAmmo = CC->GetCurrentAmmo() < CC->GetMaxAmmo() / 2;
		Intent = (bLowAmmo && AmmoP) ? EBotIntent::SeekAmmo : EBotIntent::Engage;
	}
	// Priority 3: Opportunistically grab health if hurt
	else if (HP < SeekHealthThreshold && HealthP)
	{
		Intent = EBotIntent::SeekHealth;
	}

	BB->SetValueAsEnum(BotBBKeys::CurrentIntent, static_cast<uint8>(Intent));
}