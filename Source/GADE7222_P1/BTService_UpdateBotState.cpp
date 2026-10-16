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

	// --- Target: prefer our attacker during the retaliation window ---
    AActor* Target = nullptr;
    
    if (AActor* Attacker = Bot->GetLastAttacker())
    {
    	const float TimeSinceHit = Bot->GetTimeSinceLastAttacked();
    	if (TimeSinceHit < Bot->RetaliationDuration)
    	{
    		// Don't require LOS during retaliation — we know they're out there
    		Target = Attacker;
    	}
    }
    
    // No attacker? Fall back to normal LOS-based detection
    if (!Target)
    {
    	Target = FindNearestVisibleTarget(Bot);
    }
    
    // Only report real LOS — so the fire task doesn't shoot through walls
    BB->SetValueAsBool(BotBBKeys::HasLineOfSight, HasLineOfSight(Bot, Target));
    
    // TargetActor is set regardless — the fire task will walk toward it even without LOS
    BB->SetValueAsObject(BotBBKeys::TargetActor, Target);

	// Pickups (by category, not class name)
	BB->SetValueAsObject(BotBBKeys::HealthPickup,
		FindNearestPickupByCategory(Bot, EPickupCategory::Health));
	BB->SetValueAsObject(BotBBKeys::AmmoPickup,
		FindNearestPickupByCategory(Bot, EPickupCategory::Ammo));
	
	AActor* WeaponP = FindNearestPickupByCategory(Bot, EPickupCategory::Ammo);
    BB->SetValueAsObject(BotBBKeys::AmmoPickup, WeaponP);
    
    UE_LOG(LogTemp, Warning, TEXT("[AI] Bot=%s AmmoPickup=%s WantsPickup=%d"),
        *Bot->GetName(),
        WeaponP ? *WeaponP->GetName() : TEXT("NULL"),
        Bot->CombatComponent ? Bot->CombatComponent->WantsWeaponOrAmmoPickup() : -1);

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


//-------------------------------------- Pickups----------------------------------------------------------------

AActor* UBTService_UpdateBotState::FindNearestPickupByCategory(ABaseCharacter* Bot, EPickupCategory Category) const
{
	if (!Bot) return nullptr;

	TArray<AActor*> AllPickups;
	UGameplayStatics::GetAllActorsOfClass(Bot->GetWorld(), APickupBase::StaticClass(), AllPickups);

	AActor* Best = nullptr;
	float BestDistSq = TNumericLimits<float>::Max();

	for (AActor* A : AllPickups)
	{
		APickupBase* Pickup = Cast<APickupBase>(A);
		if (!Pickup) continue;
		if (Pickup->Category != Category) continue;

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
	AActor* Target   = Cast<AActor>(BB->GetValueAsObject(BotBBKeys::TargetActor));
	AActor* HealthP  = Cast<AActor>(BB->GetValueAsObject(BotBBKeys::HealthPickup));
	AActor* WeaponP  = Cast<AActor>(BB->GetValueAsObject(BotBBKeys::AmmoPickup));
	
	EBotIntent Intent = EBotIntent::Patrol;

	// Priority 1: Retreat if critically hurt
	if (HP < FleeHealthThreshold)
	{
		Intent = HealthP ? EBotIntent::SeekHealth : EBotIntent::Flee;
	}
	// Priority 2: If engaging a target, keep engaging "
	else if (Target)
	{
		Intent = EBotIntent::Engage;
	}
	// Priority 3: No target, but we need a weapon or ammo → go get it
	else if (Bot->CombatComponent->WantsWeaponOrAmmoPickup() && WeaponP)
	{
		Intent = EBotIntent::SeekAmmo;
	}
	// Priority 4: No target, but we're hurt → grab health
	else if (HP < SeekHealthThreshold && HealthP)
	{
		Intent = EBotIntent::SeekHealth;
	}
	BB->SetValueAsEnum(BotBBKeys::CurrentIntent, static_cast<uint8>(Intent));
	UE_LOG(LogTemp, Warning, TEXT("[AI] %s Intent=%d (0=Patrol 1=Engage 2=SeekHealth 3=SeekAmmo 4=Flee)"),
	*Bot->GetName(), (int32)Intent);
}