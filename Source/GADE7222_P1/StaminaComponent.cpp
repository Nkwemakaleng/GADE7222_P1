#include "StaminaComponent.h"

UStaminaComponent::UStaminaComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UStaminaComponent::BeginPlay()
{
	Super::BeginPlay();
	CurrentStamina = MaxStamina;
	OnStaminaChanged.Broadcast(CurrentStamina, MaxStamina);
}

void UStaminaComponent::DrainStamina(float DeltaTime)
{
	if (CurrentStamina <= 0.0f) return;
	CurrentStamina = FMath::Max(CurrentStamina - DrainRate * DeltaTime, 0.0f);
	OnStaminaChanged.Broadcast(CurrentStamina, MaxStamina);
}

void UStaminaComponent::RegenerateStamina(float DeltaTime)
{
	if (CurrentStamina >= MaxStamina) return;
	CurrentStamina = FMath::Min(CurrentStamina + RegenRate * DeltaTime, MaxStamina);
	OnStaminaChanged.Broadcast(CurrentStamina, MaxStamina);
}

void UStaminaComponent::ConsumeStamina(float Amount)
{
	CurrentStamina = FMath::Max(CurrentStamina - Amount, 0.0f);
	OnStaminaChanged.Broadcast(CurrentStamina, MaxStamina);
}

void UStaminaComponent::RestoreStamina(float Amount)
{
	CurrentStamina = FMath::Min(CurrentStamina + Amount, MaxStamina);
	OnStaminaChanged.Broadcast(CurrentStamina, MaxStamina);
}