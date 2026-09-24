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

ABaseCharacter::ABaseCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	CombatComponent = CreateDefaultSubobject<UCombatComponent>(TEXT("CombatComponent"));
	HealthComponent = CreateDefaultSubobject<UHealthComponent>(TEXT("HealthComponent"));
	StaminaComponent = CreateDefaultSubobject<UStaminaComponent>(TEXT("StaminaComponent"));

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

// ---------------- INTERACTION ----------------

void ABaseCharacter::UpdateInteractionFocus()
{
	FocusedInteractable = nullptr;

	if (!PlayerCamera) return;

	FVector Start = PlayerCamera->GetComponentLocation();
	FVector End = Start + PlayerCamera->GetForwardVector() * InteractionRange;

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
	if (HealthComponent)
	{
		HealthComponent->ApplyDamage(DamageAmount, DamageCauser);
	}
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