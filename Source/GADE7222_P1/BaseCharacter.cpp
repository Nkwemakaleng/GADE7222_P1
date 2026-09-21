#include "BaseCharacter.h"
#include "CombatComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/CapsuleComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"

ABaseCharacter::ABaseCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	CombatComponent = CreateDefaultSubobject<UCombatComponent>(TEXT("CombatComponent"));

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

	UCharacterMovementComponent* MovementComp = GetCharacterMovement();
	if (MovementComp)
	{
		MovementComp->MaxWalkSpeed = MaxWalkSpeed;
		MovementComp->MaxWalkSpeedCrouched = MaxWalkSpeedCrouched;
		MovementComp->JumpZVelocity = JumpHeight;
		MovementComp->AirControl = AirControl;
		MovementComp->GroundFriction = GroundFriction;
		MovementComp->BrakingDecelerationWalking = BrakingDecelerationWalking;
		MovementComp->bOrientRotationToMovement = false;
		MovementComp->bUseControllerDesiredRotation = true;
	}
}

void ABaseCharacter::BeginPlay()
{
	Super::BeginPlay();
}

void ABaseCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ABaseCharacter::StartFire()
{
	if (CombatComponent) CombatComponent->StartFire();
}

void ABaseCharacter::StopFire()
{
	if (CombatComponent) CombatComponent->StopFire();
}

void ABaseCharacter::NextWeapon()
{
	if (CombatComponent) CombatComponent->NextWeapon();
}

void ABaseCharacter::PickupWeapon(TSubclassOf<AWeaponBase> WeaponClass, int32 AmmoAmount)
{
	if (CombatComponent) 
	{
		CombatComponent->HandleWeaponPickup(WeaponClass, AmmoAmount);
	}
}

void ABaseCharacter::MoveForward(float Value)
{
	if (Controller && Value != 0.0f)
	{
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		AddMovementInput(Direction, Value);
	}
}

void ABaseCharacter::MoveRight(float Value)
{
	if (Controller && Value != 0.0f)
	{
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
		AddMovementInput(Direction, Value);
	}
}

void ABaseCharacter::StartJump() { Jump(); }
void ABaseCharacter::StopJump() { StopJumping(); }
void ABaseCharacter::StartCrouch() { Crouch(); }
void ABaseCharacter::StopCrouch() { UnCrouch(); }