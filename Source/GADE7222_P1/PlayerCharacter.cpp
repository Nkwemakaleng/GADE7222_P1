#include "PlayerCharacter.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputActionValue.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Engine/LocalPlayer.h"

APlayerCharacter::APlayerCharacter()
{
	PrimaryActorTick.bCanEverTick = true;
}

void APlayerCharacter::BeginPlay()
{
	Super::BeginPlay();
}

void APlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if (UEnhancedInputComponent* EnhancedInput = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		if (MoveForwardAction)
			EnhancedInput->BindAction(MoveForwardAction, ETriggerEvent::Triggered, this, &APlayerCharacter::MoveForwardInput);
		if (MoveRightAction)
			EnhancedInput->BindAction(MoveRightAction, ETriggerEvent::Triggered, this, &APlayerCharacter::MoveRightInput);
		if (LookAction)
			EnhancedInput->BindAction(LookAction, ETriggerEvent::Triggered, this, &APlayerCharacter::LookInput);
		if (JumpAction)
		{
			EnhancedInput->BindAction(JumpAction, ETriggerEvent::Started, this, &APlayerCharacter::JumpInput);
			EnhancedInput->BindAction(JumpAction, ETriggerEvent::Completed, this, &APlayerCharacter::JumpInput);
		}
		if (CrouchAction)
		{
			EnhancedInput->BindAction(CrouchAction, ETriggerEvent::Started, this, &APlayerCharacter::CrouchInput);
			EnhancedInput->BindAction(CrouchAction, ETriggerEvent::Completed, this, &APlayerCharacter::CrouchInput);
		}
		if (FireAction)
		{
			EnhancedInput->BindAction(FireAction, ETriggerEvent::Started, this, &APlayerCharacter::FireInput);
			EnhancedInput->BindAction(FireAction, ETriggerEvent::Completed, this, &APlayerCharacter::FireInput);
		}
		if (SwitchWeaponAction)
		{
			EnhancedInput->BindAction(SwitchWeaponAction, ETriggerEvent::Started, this, &APlayerCharacter::SwitchWeaponInput);
		}
	}
}

void APlayerCharacter::MoveForwardInput(const FInputActionValue& Value) { MoveForward(Value.Get<float>()); }
void APlayerCharacter::MoveRightInput(const FInputActionValue& Value) { MoveRight(Value.Get<float>()); }

void APlayerCharacter::LookInput(const FInputActionValue& Value)
{
	FVector2D LookValue = Value.Get<FVector2D>();
	if (Controller)
	{
		AddControllerYawInput(LookValue.X);
		AddControllerPitchInput(LookValue.Y * -1.0f); // Fixed inversion
	}
}

void APlayerCharacter::JumpInput(const FInputActionValue& Value)
{
	if (Value.Get<bool>()) StartJump(); else StopJump();
}

void APlayerCharacter::CrouchInput(const FInputActionValue& Value)
{
	if (Value.Get<bool>()) StartCrouch(); else StopCrouch();
}

void APlayerCharacter::FireInput(const FInputActionValue& Value)
{
	if (Value.Get<bool>()) StartFire(); else StopFire();
}

void APlayerCharacter::SwitchWeaponInput(const FInputActionValue& Value)
{
	NextWeapon();
}