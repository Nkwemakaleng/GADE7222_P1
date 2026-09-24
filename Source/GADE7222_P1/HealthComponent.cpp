#include "HealthComponent.h"

UHealthComponent::UHealthComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UHealthComponent::BeginPlay()
{
	Super::BeginPlay();
	CurrentHealth = MaxHealth;
	OnHealthChanged.Broadcast(CurrentHealth, MaxHealth);
}

void UHealthComponent::ApplyDamage(float Amount, AActor* DamageCauser)
{
	if (Amount <= 0.0f || IsDead()) return;

	CurrentHealth = FMath::Clamp(CurrentHealth - Amount, 0.0f, MaxHealth);
	OnHealthChanged.Broadcast(CurrentHealth, MaxHealth);

	UE_LOG(LogTemp, Warning, TEXT("%s took %.1f damage (%.1f/%.1f)"),
		*GetOwner()->GetName(), Amount, CurrentHealth, MaxHealth);

	if (IsDead())
	{
		OnDeath.Broadcast(DamageCauser);
	}
}

void UHealthComponent::Heal(float Amount)
{
	if (Amount <= 0.0f || IsDead()) return;
	CurrentHealth = FMath::Clamp(CurrentHealth + Amount, 0.0f, MaxHealth);
	OnHealthChanged.Broadcast(CurrentHealth, MaxHealth);
}

void UHealthComponent::ResetHealth()
{
	CurrentHealth = MaxHealth;
	OnHealthChanged.Broadcast(CurrentHealth, MaxHealth);
}

float UHealthComponent::GetHealthPercent() const
{
	return MaxHealth > 0.0f ? CurrentHealth / MaxHealth : 0.0f;
}