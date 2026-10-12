#include "PickupBase.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/RotatingMovementComponent.h"
#include "TimerManager.h"
#include "Engine/World.h"

APickupBase::APickupBase()
{
	PrimaryActorTick.bCanEverTick = true;

	// --- Collision sphere as root ---
	PickupSphere = CreateDefaultSubobject<USphereComponent>(TEXT("PickupSphere"));
	PickupSphere->InitSphereRadius(60.0f);
	PickupSphere->SetCollisionProfileName(TEXT("OverlapAllDynamic"));
	PickupSphere->SetGenerateOverlapEvents(true);
	RootComponent = PickupSphere;

	// --- Visual mesh ---
	PickupMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("PickupMesh"));
	PickupMesh->SetupAttachment(PickupSphere);
	PickupMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	PickupMesh->SetGenerateOverlapEvents(false);

	// --- Rotating movement (optional) ---
	RotatingMovement = CreateDefaultSubobject<URotatingMovementComponent>(TEXT("RotatingMovement"));
	RotatingMovement->RotationRate = FRotator(0.0f, 90.0f, 0.0f);
	RotatingMovement->bAutoActivate = true;

	// Overlap binding
	PickupSphere->OnComponentBeginOverlap.AddDynamic(this, &APickupBase::OnSphereOverlap);
}

void APickupBase::BeginPlay()
{
	Super::BeginPlay();

	InitialLocation = GetActorLocation();

	// Configure optional behaviours
	if (RotatingMovement)
	{
		RotatingMovement->SetActive(bRotate);
	}
}

void APickupBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (bBob)
	{
		BobTimer += DeltaTime * BobSpeed;
		FVector Offset(0.0f, 0.0f, FMath::Sin(BobTimer) * BobHeight);
		SetActorLocation(InitialLocation + Offset);
	}
}

// ---------------- INTERFACE ----------------

void APickupBase::Interact_Implementation(AActor* Interactor)
{
	TryConsume(Interactor);
}

FText APickupBase::GetInteractPrompt_Implementation() const
{
	return InteractPrompt;
}

bool APickupBase::CanInteract_Implementation(AActor* Interactor) const
{
	if (bIsOnCooldown) return false;
	return CanBePickedUp(Interactor);
}

// ---------------- OVERLAP ----------------

void APickupBase::OnSphereOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (!bAutoPickupOnOverlap) return;
	if (!OtherActor || OtherActor == this) return;

	TryConsume(OtherActor);
}

// ---------------- CORE LOGIC ----------------

bool APickupBase::TryConsume(AActor* Interactor)
{
	if (!Interactor) return false;
	if (bIsOnCooldown) return false;

	// Child's gate
	if (!CanBePickedUp(Interactor)) return false;

	// Child's action
	const bool bConsumed = OnPickedUp(Interactor);
	if (!bConsumed) return false;
	
	//Notify Listener spawner and hud
	OnPickupConsumed.Broadcast(this, Interactor);
	
	// Destroy or start cooldown
	if (bDestroyOnPickup)
	{
		Destroy();
	}
	else if (ReuseCooldown > 0.0f)
	{
		bIsOnCooldown = true;
		SetPickupActive(false);

		GetWorldTimerManager().SetTimer(CooldownHandle, [this]()
		{
			bIsOnCooldown = false;
			SetPickupActive(true);
		}, ReuseCooldown, false);
	}

	return true;
}

void APickupBase::SetPickupActive(bool bActive)
{
	SetActorHiddenInGame(!bActive);
	PickupSphere->SetCollisionEnabled(bActive ? ECollisionEnabled::QueryOnly : ECollisionEnabled::NoCollision);

	if (RotatingMovement)
	{
		RotatingMovement->SetActive(bActive && bRotate);
	}
}

// ---------------- DEFAULTS FOR CHILDREN ----------------

bool APickupBase::OnPickedUp_Implementation(AActor* Interactor)
{
	// Default: nothing happens. Children override.
	UE_LOG(LogTemp, Warning, TEXT("%s picked up (no effect implemented)."), *GetName());
	return true;
}

bool APickupBase::CanBePickedUp_Implementation(AActor* Interactor) const
{
	// Default: always allowed. Children override to gate (e.g. full HP).
	return true;
}