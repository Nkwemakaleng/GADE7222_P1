// PlayerCharacter.cpp
#include "PlayerCharacter.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputActionValue.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Engine/LocalPlayer.h"

APlayerCharacter::APlayerCharacter()
{
	PrimaryActorTick.bCanEverTick = true;
	
	// Ensure the character can move
	AutoPossessPlayer = EAutoReceiveInput::Player0;
}

void APlayerCharacter::BeginPlay()
{
	Super::BeginPlay();

	// Log to verify the character is being created
	UE_LOG(LogTemp, Warning, TEXT("PlayerCharacter BeginPlay called - IsLocallyControlled: %d"), IsLocallyControlled());

	if (APlayerController* PlayerController = Cast<APlayerController>(GetController()))
	{
		UE_LOG(LogTemp, Warning, TEXT("PlayerCharacter: Controller found"));
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("PlayerCharacter: No Controller!"));
	}
	
	// Force movement settings again
	if (UCharacterMovementComponent* MovementComp = GetCharacterMovement())
	{
		MovementComp->MaxWalkSpeed = 600.0f;
		MovementComp->MaxAcceleration = 4096.0f;
		UE_LOG(LogTemp, Warning, TEXT("PlayerCharacter: Movement MaxWalkSpeed: %f"), MovementComp->MaxWalkSpeed);
	}
}

void APlayerCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	// Debug: Log velocity every few seconds
	static float Timer = 0.0f;
	Timer += DeltaTime;
	if (Timer > 2.0f)
	{
		Timer = 0.0f;
		if (UCharacterMovementComponent* MovementComp = GetCharacterMovement())
		{
			if (!MovementComp->Velocity.IsNearlyZero())
			{
				UE_LOG(LogTemp, Warning, TEXT("Moving! Velocity: %s"), *MovementComp->Velocity.ToString());
			}
		}
	}
}

void APlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	UE_LOG(LogTemp, Warning, TEXT("SetupPlayerInputComponent called"));

	if (UEnhancedInputComponent* EnhancedInput = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		// Bind movement actions
		if (MoveForwardAction)
		{
			EnhancedInput->BindAction(MoveForwardAction, ETriggerEvent::Triggered, this, &APlayerCharacter::MoveForwardInput);
			UE_LOG(LogTemp, Warning, TEXT("MoveForwardAction bound"));
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("MoveForwardAction is null! Please assign in Blueprint."));
		}

		if (MoveRightAction)
		{
			EnhancedInput->BindAction(MoveRightAction, ETriggerEvent::Triggered, this, &APlayerCharacter::MoveRightInput);
			UE_LOG(LogTemp, Warning, TEXT("MoveRightAction bound"));
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("MoveRightAction is null! Please assign in Blueprint."));
		}

		if (LookAction)
		{
			EnhancedInput->BindAction(LookAction, ETriggerEvent::Triggered, this, &APlayerCharacter::LookInput);
			UE_LOG(LogTemp, Warning, TEXT("LookAction bound"));
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("LookAction is null! Please assign in Blueprint."));
		}

		if (JumpAction)
		{
			EnhancedInput->BindAction(JumpAction, ETriggerEvent::Started, this, &APlayerCharacter::JumpInput);
			EnhancedInput->BindAction(JumpAction, ETriggerEvent::Completed, this, &APlayerCharacter::JumpInput);
			UE_LOG(LogTemp, Warning, TEXT("JumpAction bound"));
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("JumpAction is null! Please assign in Blueprint."));
		}

		if (CrouchAction)
		{
			EnhancedInput->BindAction(CrouchAction, ETriggerEvent::Started, this, &APlayerCharacter::CrouchInput);
			EnhancedInput->BindAction(CrouchAction, ETriggerEvent::Completed, this, &APlayerCharacter::CrouchInput);
			UE_LOG(LogTemp, Warning, TEXT("CrouchAction bound"));
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("CrouchAction is null! Please assign in Blueprint."));
		}
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to cast to EnhancedInputComponent! Make sure Enhanced Input plugin is enabled."));
	}
}

void APlayerCharacter::MoveForwardInput(const FInputActionValue& Value)
{
	float MovementValue = Value.Get<float>();
	UE_LOG(LogTemp, Verbose, TEXT("MoveForwardInput: %f"), MovementValue);
	MoveForward(MovementValue);
}

void APlayerCharacter::MoveRightInput(const FInputActionValue& Value)
{
	float MovementValue = Value.Get<float>();
	UE_LOG(LogTemp, Verbose, TEXT("MoveRightInput: %f"), MovementValue);
	MoveRight(MovementValue);
}

void APlayerCharacter::JumpInput(const FInputActionValue& Value)
{
	bool bPressed = Value.Get<bool>();
	UE_LOG(LogTemp, Verbose, TEXT("JumpInput: %d"), bPressed);
	if (bPressed)
	{
		StartJump();
	}
	else
	{
		StopJump();
	}
}

void APlayerCharacter::CrouchInput(const FInputActionValue& Value)
{
	bool bPressed = Value.Get<bool>();
	UE_LOG(LogTemp, Verbose, TEXT("CrouchInput: %d"), bPressed);
	if (bPressed)
	{
		StartCrouch();
	}
	else
	{
		StopCrouch();
	}
}

void APlayerCharacter::LookInput(const FInputActionValue& Value)
{
	FVector2D LookValue = Value.Get<FVector2D>();
	
	if (Controller)
	{
		// Mouse X - Yaw (left/right)
		AddControllerYawInput(LookValue.X);
		
		// Mouse Y - Pitch (up/down) - INVERTED 
		AddControllerPitchInput(LookValue.Y * -1.0f);
		
		UE_LOG(LogTemp, Verbose, TEXT("LookInput: X=%f, Y=%f"), LookValue.X, LookValue.Y);
	}
}