#include "BaseCharacter.h"
#include "CombatComponent.h"
#include "HealthComponent.h"
#include "StaminaComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/CapsuleComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "InteractableInterface.h"
#include "Engine/World.h"
#include "TimerManager.h"
#include "EngineUtils.h"
#include "GameFramework/PlayerStart.h"
#include "Kismet/GameplayStatics.h"
#include "Components/WidgetComponent.h"
#include "HealthBarWidget.h"

ABaseCharacter::ABaseCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	// Set up Components
	CombatComponent = CreateDefaultSubobject<UCombatComponent>(TEXT("CombatComponent"));
	HealthComponent = CreateDefaultSubobject<UHealthComponent>(TEXT("HealthComponent"));
	StaminaComponent = CreateDefaultSubobject<UStaminaComponent>(TEXT("StaminaComponent"));

	//Set up Camera
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 0.0f;
	CameraBoom->bUsePawnControlRotation = true;
	CameraBoom->bInheritPitch = true;
	CameraBoom->bInheritYaw = true;
	CameraBoom->bInheritRoll = false;

	PlayerCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("PlayerCamera"));
	PlayerCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	PlayerCamera->bUsePawnControlRotation = false;

	// Setup Health Bar
	HealthBarWidgetComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("HealthBarWidget"));
	HealthBarWidgetComponent->SetupAttachment(RootComponent);
	HealthBarWidgetComponent->SetWidgetSpace(EWidgetSpace::Screen); // Renders in screen space
	HealthBarWidgetComponent->SetRelativeLocation(FVector(0.f, 0.f, 110.f)); // 110 units above origin (head height)
	HealthBarWidgetComponent->SetDrawSize(FVector2D(120.f, 15.f));
	HealthBarWidgetComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	
	//Allows the AI Model to rotate to face the player
	bUseControllerRotationYaw = true;
	bUseControllerRotationPitch = false;
	bUseControllerRotationRoll = false;
	
	UCharacterMovementComponent* CMC = GetCharacterMovement();
	if (CMC)
	{
		CMC->MaxWalkSpeed = WalkSpeed;
		CMC->MaxWalkSpeedCrouched = MaxWalkSpeedCrouched;
		CMC->JumpZVelocity = JumpHeight;
		CMC->AirControl = AirControl;
		CMC->GroundFriction = GroundFriction;
		CMC->BrakingDecelerationWalking = BrakingDecelerationWalking;
		CMC->MaxAcceleration = 2048.0f;
		CMC->bOrientRotationToMovement = false;
		CMC->bUseControllerDesiredRotation = true;
	}
	
}

void ABaseCharacter::BeginPlay()
{
	Super::BeginPlay();

	if (CombatComponent)
	{
		CombatComponent->InitializeWithWeapons(StartingWeapons);
	}
	if (HealthComponent)
	{
		HealthComponent->OnDeath.AddDynamic(this, &ABaseCharacter::HandleDeath);
	}
	// Setup Health Bar Widget
	if (HealthBarWidgetComponent && HealthBarWidgetClass)
	{
		HealthBarWidgetComponent->SetWidgetClass(HealthBarWidgetClass);
		UHealthBarWidget* Widget = Cast<UHealthBarWidget>(HealthBarWidgetComponent->GetUserWidgetObject());
		if (Widget && HealthComponent)
		{
			Widget->SetHealthComponent(HealthComponent);
		}
	}
	else
	{
		// If no widget class is set, hide it by default
		HealthBarWidgetComponent->SetVisibility(false);
	}
}

void ABaseCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	TickSprint(DeltaTime);
}

// ---------------- SPRINT ----------------

void ABaseCharacter::StartSprint()
{
	if (!StaminaComponent || !StaminaComponent->CanStartSprint()) return;

	bIsSprinting = true;
	bCanRegenStamina = false;

	if (UCharacterMovementComponent* CMC = GetCharacterMovement())
	{
		CMC->MaxWalkSpeed = SprintSpeed;
	}

	GetWorldTimerManager().ClearTimer(RegenDelayTimer);
}

void ABaseCharacter::StopSprint()
{
	if (!bIsSprinting) return;

	bIsSprinting = false;

	if (UCharacterMovementComponent* CMC = GetCharacterMovement())
	{
		CMC->MaxWalkSpeed = WalkSpeed;
	}

	// Start regen delay
	if (UWorld* W = GetWorld())
	{
		W->GetTimerManager().SetTimer(RegenDelayTimer, [this]()
		{
			bCanRegenStamina = true;
		}, StaminaComponent ? StaminaComponent->RegenDelay : 1.5f, false);
	}
}

void ABaseCharacter::TickSprint(float DeltaTime)
{
	if (!StaminaComponent) return;

	if (bIsSprinting)
	{
		StaminaComponent->DrainStamina(DeltaTime);

		// Auto-stop when depleted
		if (!StaminaComponent->HasStamina())
		{
			StopSprint();
		}
	}
	else if (bCanRegenStamina)
	{
		StaminaComponent->RegenerateStamina(DeltaTime);
	}
}

// ---------------- IINTERACTION ----------------

void ABaseCharacter::UpdateInteractionFocus()
{
	FocusedInteractable = nullptr;

	// Prefer camera (players), fall back to controller view (AI)
	FVector Start;
	FVector Forward;

	if (PlayerCamera)
	{
		Start = PlayerCamera->GetComponentLocation();
		Forward = PlayerCamera->GetForwardVector();
	}
	else if (Controller)
	{
		FRotator ViewRot;
		Controller->GetPlayerViewPoint(Start, ViewRot);
		Forward = ViewRot.Vector();
	}
	else return;

	FVector End = Start + Forward * InteractionRange;

	FHitResult Hit;
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(this);
	Params.bTraceComplex = true;

	if (GetWorld()->LineTraceSingleByChannel(Hit, Start, End, ECC_Visibility, Params))
	{
		AActor* HitActor = Hit.GetActor();
		if (HitActor && HitActor->GetClass()->ImplementsInterface(UInteractableInterface::StaticClass()))
		{
			FocusedInteractable = HitActor;
		}
	}
}

void ABaseCharacter::TryInteract()
{
	UpdateInteractionFocus();

	if (!FocusedInteractable) return;

	if (IInteractableInterface* Interactable = Cast<IInteractableInterface>(FocusedInteractable))
	{
		if (FocusedInteractable->GetClass()->ImplementsInterface(UInteractableInterface::StaticClass()))
		{
			// Check CanInteract
			if (IInteractableInterface::Execute_CanInteract(FocusedInteractable, this))
			{
				IInteractableInterface::Execute_Interact(FocusedInteractable, this);
			}
		}
	}
}

// ---------------- COMBAT WRAPPERS ----------------

void ABaseCharacter::StartFire() { if (CombatComponent) CombatComponent->StartFire(); }
void ABaseCharacter::StopFire()  { if (CombatComponent) CombatComponent->StopFire(); }
void ABaseCharacter::NextWeapon(){ if (CombatComponent) CombatComponent->NextWeapon(); }

void ABaseCharacter::PickupWeapon(TSubclassOf<AWeaponBase> WeaponClass, int32 AmmoAmount)
{
	if (CombatComponent) CombatComponent->HandleWeaponPickup(WeaponClass, AmmoAmount);
}

// ---------------- IDamageable ----------------

void ABaseCharacter::ReceiveDamage_Implementation(float DamageAmount, AActor* DamageCauser, const FVector& HitLocation)
{
	// Ignore if already dead
	if (HealthComponent && HealthComponent->IsDead()) return;

	// --- Retaliation: remember who hit us ---
	if (DamageCauser && DamageCauser != this)
	{
		LastAttacker = DamageCauser;
		LastAttackTime = GetWorld()->TimeSeconds;
		UE_LOG(LogTemp, Warning, TEXT("%s was hit by %s. Retaliation locked."), *GetName(), *DamageCauser->GetName());
	}

	// --- Broadcast for listeners (HUD, quests, etc.) ---
	OnDamaged.Broadcast(DamageAmount, DamageCauser);

	// --- Apply the damage ---
	if (HealthComponent)
	{
		HealthComponent->ApplyDamage(DamageAmount, DamageCauser);
	}
}

AActor* ABaseCharacter::GetLastAttacker() const
{
	// Forget the attacker if it's been too long
	if (GetWorld() && (GetWorld()->TimeSeconds - LastAttackTime) > RetaliationDuration)
	{
		return nullptr;
	}
	return LastAttacker.Get();
}

float ABaseCharacter::GetTimeSinceLastAttacked() const
{
	if (!GetWorld()) return 9999.f;
	return GetWorld()->TimeSeconds - LastAttackTime;
}


float ABaseCharacter::GetCurrentHealth_Implementation() const
{
	return HealthComponent ? HealthComponent->CurrentHealth : 0.0f;
}

bool ABaseCharacter::IsAlive_Implementation() const
{
	return HealthComponent ? !HealthComponent->IsDead() : false;
}

// ---------------- MOVEMENT ----------------

void ABaseCharacter::MoveForward(float Value)
{
	if (!FMath::IsFinite(Value) || FMath::Abs(Value) < 0.01f) return;
	if (Controller)
	{
		const FRotator R = Controller->GetControlRotation();
		const FRotator Y(0, R.Yaw, 0);
		AddMovementInput(FRotationMatrix(Y).GetUnitAxis(EAxis::X), Value);
	}
}

void ABaseCharacter::MoveRight(float Value)
{
	if (!FMath::IsFinite(Value) || FMath::Abs(Value) < 0.01f) return;
	if (Controller)
	{
		const FRotator R = Controller->GetControlRotation();
		const FRotator Y(0, R.Yaw, 0);
		AddMovementInput(FRotationMatrix(Y).GetUnitAxis(EAxis::Y), Value);
	}
}

void ABaseCharacter::StartJump() { Jump(); }
void ABaseCharacter::StopJump() { StopJumping(); }
void ABaseCharacter::StartCrouch() { Crouch(); }
void ABaseCharacter::StopCrouch() { UnCrouch(); }

// ---------------- RESPAWN ----------------
void ABaseCharacter::HandleDeath(AActor* Killer)
{
	UE_LOG(LogTemp, Warning, TEXT("%s died (killed by %s)"),
		*GetName(), Killer ? *Killer->GetName() : TEXT("world"));

	// Disable input, hide, stop movement
	if (APlayerController* PC = Cast<APlayerController>(GetController()))
	{
		DisableInput(PC);
	}
	
	if (UCharacterMovementComponent* CMC = GetCharacterMovement())
	{
		CMC->DisableMovement();
	}

	// --- Destroy the equipped weapon so it disappears with the character ---
	if (CombatComponent)
	{
		CombatComponent->DestroyAllWeapons();
	}
	SetActorHiddenInGame(true);
	SetActorEnableCollision(false);
	SetHealthBarVisible(false);

	GetWorldTimerManager().SetTimer(RespawnTimerHandle,
		this, &ABaseCharacter::Respawn, RespawnDelay, false);
}
//Handles Respawning via player start actor
void ABaseCharacter::Respawn()
{
	AActor* Point = FindRandomRespawnPoint();
	if (!Point) return;

	SetActorLocation(Point->GetActorLocation(), false, nullptr, ETeleportType::TeleportPhysics);
	SetActorRotation(Point->GetActorRotation());
	SetActorHiddenInGame(false);
	SetActorEnableCollision(true);
	GetCharacterMovement()->SetMovementMode(MOVE_Walking);

	if (HealthComponent) HealthComponent->ResetHealth();
	if (StaminaComponent) StaminaComponent->RestoreStamina(StaminaComponent->MaxStamina);

	// Re-equip the starting weapon
	if (CombatComponent)
	{
		CombatComponent->InitializeWithWeapons(StartingWeapons);
	}

	if (APlayerController* PC = Cast<APlayerController>(GetController()))
	{
		EnableInput(PC);
	}
	SetHealthBarVisible(true);
}

AActor* ABaseCharacter::FindRandomRespawnPoint() const
{
	TArray<AActor*> Points;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), APlayerStart::StaticClass(), Points);

	if (Points.Num() == 0) return nullptr;
	return Points[FMath::RandRange(0, Points.Num() - 1)];
}

bool ABaseCharacter::CanDash() const
{
	if (bIsDashing) return false;

	UWorld* World = GetWorld();
	if (!World) return false;

	if (World->TimeSeconds - LastDashTime < DashCooldown) return false;

	// Air dash check
	if (!bCanAirDash)
	{
		if (const UCharacterMovementComponent* CMC = GetCharacterMovement())
		{
			if (CMC->IsFalling()) return false;
		}
	}

	// Stamina check
	if (StaminaComponent && DashStaminaCost > 0.f)
	{
		if (StaminaComponent->CurrentStamina < DashStaminaCost) return false;
	}

	return true;
}

void ABaseCharacter::TryDash(const FVector& Direction)
{
	if (!CanDash()) return;

	// --- Resolve direction ---
	FVector Dir = Direction;
	if (Dir.IsNearlyZero())
	{
		// Prefer current velocity's horizontal direction
		Dir = GetVelocity();
		Dir.Z = 0.f;

		// Not moving? Fall back to controller's facing
		if (Dir.SizeSquared() < 100.f)
		{
			Dir = GetControlRotation().Vector();
			Dir.Z = 0.f;
		}
	}

	Dir = Dir.GetSafeNormal();

	// --- Consume stamina ---
	if (StaminaComponent && DashStaminaCost > 0.f)
	{
		StaminaComponent->ConsumeStamina(DashStaminaCost);
	}

	// --- Apply impulse ---
	bIsDashing = true;
	LastDashTime = GetWorld()->TimeSeconds;

	// LaunchCharacter replaces XY velocity, preserves Z
	LaunchCharacter(Dir * DashSpeed, /*bXYOverride=*/ true, /*bZOverride=*/ false);

	// Reset the "dashing" flag after DashDuration
	GetWorldTimerManager().SetTimer(DashEndHandle, this, &ABaseCharacter::EndDash, DashDuration, false);

	UE_LOG(LogTemp, Verbose, TEXT("%s dashed toward %s"), *GetName(), *Dir.ToString());
}

void ABaseCharacter::EndDash()
{
	bIsDashing = false;
}

void ABaseCharacter::SetHealthBarVisible(bool bVisible)
{
	if (HealthBarWidgetComponent)
	{
		HealthBarWidgetComponent->SetVisibility(bVisible);
	}
}