#include "PickupSpawner.h"
#include "PickupBase.h"
#include "Components/BillboardComponent.h"
#include "Components/ArrowComponent.h"
#include "Engine/World.h"
#include "TimerManager.h"
#include "DrawDebugHelpers.h"

APickupSpawner::APickupSpawner()
{
	PrimaryActorTick.bCanEverTick = false;

	// Root — an empty scene component
	USceneComponent* Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	RootComponent = Root;

	// Editor icon
	EditorIcon = CreateDefaultSubobject<UBillboardComponent>(TEXT("EditorIcon"));
	EditorIcon->SetupAttachment(RootComponent);
	EditorIcon->bIsEditorOnly = true;

	// Arrow showing spawn direction
	HeightArrow = CreateDefaultSubobject<UArrowComponent>(TEXT("HeightArrow"));
	HeightArrow->SetupAttachment(RootComponent);
	HeightArrow->ArrowSize = 1.5f;
	HeightArrow->ArrowColor = FColor::Green;
	HeightArrow->bIsEditorOnly = true;
}

void APickupSpawner::BeginPlay()
{
	Super::BeginPlay();

	if (bSpawnOnBeginPlay)
	{
		SpawnPickupNow();
	}
}

void APickupSpawner::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	bShuttingDown = true;
	GetWorldTimerManager().ClearTimer(RespawnTimerHandle);
	Super::EndPlay(EndPlayReason);
}

#if WITH_EDITOR
void APickupSpawner::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);

	// Update the arrow length so it visually represents SpawnHeight
	if (HeightArrow)
	{
		HeightArrow->ArrowSize = FMath::Max(SpawnHeight / 100.0f, 0.5f);
	}

	// Draw a debug sphere at the spawn point while in the editor viewport
	DrawDebugSphere(GetWorld(), GetActorLocation() + FVector(0, 0, SpawnHeight),
		20.0f, 12, FColor::Green, false, -1.0f, 0, 1.0f);
}
#endif

APickupBase* APickupSpawner::SpawnPickupNow()
{
	if (!PickupClass)
	{
		UE_LOG(LogTemp, Error, TEXT("PickupSpawner %s: PickupClass is NULL!"), *GetName());
		return nullptr;
	}

	// Enforce concurrent limit
	if (ActivePickups.Num() >= MaxActivePickups)
	{
		return nullptr;
	}

	// Compute spawn location: own position + height + optional horizontal jitter
	FVector SpawnLoc = GetActorLocation() + FVector(0.0f, 0.0f, SpawnHeight);

	if (RandomHorizontalOffset > 0.0f)
	{
		SpawnLoc.X += FMath::FRandRange(-RandomHorizontalOffset, RandomHorizontalOffset);
		SpawnLoc.Y += FMath::FRandRange(-RandomHorizontalOffset, RandomHorizontalOffset);
	}

	FActorSpawnParameters Params;
	Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	Params.Owner = this;

	APickupBase* Spawned = GetWorld()->SpawnActor<APickupBase>(
		PickupClass, SpawnLoc, FRotator::ZeroRotator, Params);

	if (Spawned)
	{
		ActivePickups.Add(Spawned);

		//OnPickupConsumed fires the "it was picked up" event
		Spawned->OnPickupConsumed.AddDynamic(this, &APickupSpawner::HandlePickupConsumed);
		// Listen for its destruction so we can respawn
		Spawned->OnDestroyed.AddDynamic(this, &APickupSpawner::HandleSpawnedPickupDestroyed);
		// Notify listeners
		OnPickupSpawned.Broadcast(Spawned);

		UE_LOG(LogTemp, Warning, TEXT("PickupSpawner: spawned %s at %s"),
			*Spawned->GetName(), *SpawnLoc.ToString());
	}

	return Spawned;
}

void APickupSpawner::HandleSpawnedPickupDestroyed(AActor* DestroyedActor)
{
	if (bShuttingDown) return;

	// Remove from tracking
	ActivePickups.RemoveAll([DestroyedActor](APickupBase* P)
	{
		return !IsValid(P) || P == DestroyedActor;
	});
}

void APickupSpawner::ScheduleRespawn()
{
	GetWorldTimerManager().ClearTimer(RespawnTimerHandle);

	const float Delay = FMath::Max(RespawnDelay, 0.01f);
	GetWorldTimerManager().SetTimer(RespawnTimerHandle, [this]()
	{
		if (!bShuttingDown)
		{
			SpawnPickupNow();
		}
	}, Delay, false);
}

void APickupSpawner::HandlePickupConsumed(APickupBase* Pickup, AActor* Interactor)
{
	UE_LOG(LogTemp, Warning, TEXT("PickupSpawner: %s was picked up by %s"),
		Pickup ? *Pickup->GetName() : TEXT("NULL"),
		Interactor ? *Interactor->GetName() : TEXT("NULL"));

	// Re-broadcast so external systems (HUD, quests, audio) can react too
	OnPickupPickedUp.Broadcast(Pickup, Interactor);
	// If the pickup will be destroyed, respawn after RespawnDelay.
	// If it's a reusable pickup (cooldown-based), the spawner itself
	// has nothing to do — the pickup comes back on its own.
	if (Pickup && Pickup->bDestroyOnPickup && RespawnDelay >= 0.0f)
	{
		ScheduleRespawn();
	}
}