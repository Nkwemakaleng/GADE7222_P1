// BaseCharacter.cpp
#include "BaseCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/CapsuleComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"

ABaseCharacter::ABaseCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	// Create camera boom
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 0.0f;
	CameraBoom->bUsePawnControlRotation = true;
	CameraBoom->bInheritPitch = true;
	CameraBoom->bInheritYaw = true;
	CameraBoom->bInheritRoll = false;

	// Create camera
	PlayerCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("PlayerCamera"));
	PlayerCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	PlayerCamera->bUsePawnControlRotation = false;

	// Configure character movement
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
	
	// Force movement settings
	if (UCharacterMovementComponent* MovementComp = GetCharacterMovement())
	{
		MovementComp->MaxWalkSpeed = 600.0f;
		MovementComp->MaxAcceleration = 4096.0f;
		MovementComp->BrakingDecelerationWalking = 500.0f;
		MovementComp->GroundFriction = 8.0f;
		MovementComp->bUseControllerDesiredRotation = true;
		
		UE_LOG(LogTemp, Warning, TEXT("BaseCharacter: Movement configured - MaxWalkSpeed: %f"), MovementComp->MaxWalkSpeed);
	}
}

void ABaseCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ABaseCharacter::MoveForward(float Value)
{
	if (Controller && Value != 0.0f)
	{
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		AddMovementInput(Direction, Value);
		
		UE_LOG(LogTemp, Verbose, TEXT("MoveForward: Value=%f, Direction=%s"), Value, *Direction.ToString());
	}
	else if (Value != 0.0f)
	{
		UE_LOG(LogTemp, Error, TEXT("MoveForward: No Controller!"));
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
		
		UE_LOG(LogTemp, Verbose, TEXT("MoveRight: Value=%f, Direction=%s"), Value, *Direction.ToString());
	}
	else if (Value != 0.0f)
	{
		UE_LOG(LogTemp, Error, TEXT("MoveRight: No Controller!"));
	}
}

void ABaseCharacter::StartJump()
{
	Jump();
	UE_LOG(LogTemp, Warning, TEXT("Jump started"));
}

void ABaseCharacter::StopJump()
{
	StopJumping();
	UE_LOG(LogTemp, Warning, TEXT("Jump stopped"));
}

void ABaseCharacter::StartCrouch()
{
	Crouch();
	UE_LOG(LogTemp, Warning, TEXT("Crouch started"));
}

void ABaseCharacter::StopCrouch()
{
	UnCrouch();
	UE_LOG(LogTemp, Warning, TEXT("Crouch stopped"));
}

void ABaseCharacter::LogMovementStatus()
{
	if (UCharacterMovementComponent* MovementComp = GetCharacterMovement())
	{
		UE_LOG(LogTemp, Warning, TEXT("Velocity: %s, MaxWalkSpeed: %f, IsMoving: %d"), 
			*MovementComp->Velocity.ToString(), 
			MovementComp->MaxWalkSpeed, 
			!MovementComp->Velocity.IsNearlyZero());
	}
}