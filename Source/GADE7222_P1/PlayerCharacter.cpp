#include "PlayerCharacter.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputActionValue.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Engine/LocalPlayer.h"
#include "MyPlayerController.h"
#include "CombatComponent.h"
#include "HUDWidget.h"
#include "Blueprint/UserWidget.h"

APlayerCharacter::APlayerCharacter()
{
	PrimaryActorTick.bCanEverTick = true;
}

void APlayerCharacter::BeginPlay()
{
	Super::BeginPlay();
	// Attaches the hud to the player
	if (HUDWidgetClass)
	{
		HUDWidget = CreateWidget<UHUDWidget>(GetWorld(), HUDWidgetClass);
		if (HUDWidget)
		{
			HUDWidget->AddToViewport();
			HUDWidget->BindToCharacter(this);   // was SetCombatComponent + SetTrackedCharacter
		}
	}
}

void APlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	// --- Pull input actions from the controller (which owns them in Blueprint) ---
	if (AMyPlayerController* PC = Cast<AMyPlayerController>(GetController()))
	{
		if (!MoveForwardAction)	MoveForwardAction  = PC->MoveForwardAction;
		if (!MoveRightAction) MoveRightAction  = PC->MoveRightAction;
		if (!LookAction) LookAction  = PC->LookAction;
		if (!JumpAction) JumpAction = PC->JumpAction;
		if (!CrouchAction)  CrouchAction  = PC->CrouchAction;
		if (!FireAction)  FireAction  = PC->FireAction;
		if (!SwitchWeaponAction) SwitchWeaponAction = PC->SwitchWeaponAction;
		if (!WeaponSlot1Action) WeaponSlot1Action = PC->WeaponSlot1Action;
		if (!WeaponSlot2Action) WeaponSlot2Action = PC->WeaponSlot2Action;
		if (!SprintAction) SprintAction = PC->SprintAction;
		if (!InteractAction) InteractAction = PC->InteractAction;
		if (!DashAction) DashAction = PC->DashAction;
		UE_LOG(LogTemp, Warning, TEXT("PlayerCharacter: Pulled input actions from controller. MoveForward=%s"),
			MoveForwardAction ? TEXT("Valid") : TEXT("NULL"));
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("PlayerCharacter: Controller is not AMyPlayerController!"));
	}
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
		if (WeaponSlot1Action)
		{
			EnhancedInput->BindAction(WeaponSlot1Action, ETriggerEvent::Started, this, &APlayerCharacter::WeaponSlot1Input);
		}
		if (WeaponSlot2Action)
		{
			EnhancedInput->BindAction(WeaponSlot2Action, ETriggerEvent::Started, this, &APlayerCharacter::WeaponSlot2Input);
		}
		if (SprintAction)
		{
			EnhancedInput->BindAction(SprintAction, ETriggerEvent::Started, this, &APlayerCharacter::SprintInput);
			EnhancedInput->BindAction(SprintAction, ETriggerEvent::Completed, this, &APlayerCharacter::SprintInput);
		}
		if (InteractAction)
		{
			EnhancedInput->BindAction(InteractAction, ETriggerEvent::Started, this, &APlayerCharacter::InteractInput);
		}
		if (DashAction)
		{
			EnhancedInput->BindAction(DashAction, ETriggerEvent::Started, this, &APlayerCharacter::DashInput);
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
		AddControllerPitchInput(LookValue.Y * -1.0f); 
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
void APlayerCharacter::WeaponSlot1Input(const FInputActionValue& Value)
{
	
	if (CombatComponent)
	{
		
		// Only switch if the slot exists
		if (CombatComponent->HasWeaponInSlot(0))
		{
			CombatComponent->SwitchWeapon(0);
			UE_LOG(LogTemp, Warning, TEXT("Switching to weapon slot 1"));
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("No weapon in slot 1"));
		}	
	}
}

void APlayerCharacter::WeaponSlot2Input(const FInputActionValue& Value)
{
	if (CombatComponent->HasWeaponInSlot(1))
	{
		CombatComponent->SwitchWeapon(1);  // slot 2 = index 1
		UE_LOG(LogTemp, Warning, TEXT("Switching to weapon slot 2"));
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("No weapon in slot 2"));
	}
}
void APlayerCharacter::SprintInput(const FInputActionValue& Value)
{
	if (Value.Get<bool>()) StartSprint(); else StopSprint();
}

void APlayerCharacter::InteractInput(const FInputActionValue& Value)
{
	TryInteract();
}
void APlayerCharacter::DashInput(const FInputActionValue& Value)
{
	// Dash in the direction the player is currently moving
	// (falls back to facing forward if stationary — handled in TryDash)
	TryDash(FVector::ZeroVector);
}