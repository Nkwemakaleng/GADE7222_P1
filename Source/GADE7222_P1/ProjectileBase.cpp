#include "ProjectileBase.h"
#include "Components/SphereComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "BaseCharacter.h"
#include "DamageableInterface.h"

AProjectileBase::AProjectileBase()
{
	PrimaryActorTick.bCanEverTick = false;

	CollisionComp = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComp"));
	CollisionComp->InitSphereRadius(5.0f);
	CollisionComp->SetCollisionProfileName(TEXT("BlockAllDynamic"));
	RootComponent = CollisionComp;

	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileComp"));
	ProjectileMovement->UpdatedComponent = CollisionComp;
	ProjectileMovement->InitialSpeed = 3000.f;
	ProjectileMovement->MaxSpeed = 3000.f;
	ProjectileMovement->bRotationFollowsVelocity = true;
	ProjectileMovement->bShouldBounce = false;

	CollisionComp->OnComponentBeginOverlap.AddDynamic(this, &AProjectileBase::OnOverlap);
}

void AProjectileBase::BeginPlay()
{
	Super::BeginPlay();
	SetLifeSpan(LifeSpan);
}

void AProjectileBase::OnOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (!OtherActor || OtherActor == this || OtherActor == GetOwner()) return;
	// Ignore other projectiles so shotgun pellets don't destroy each other on spawn
	if (OtherActor->IsA<AProjectileBase>()) return; 

	if (OtherActor->GetClass()->ImplementsInterface(UDamageableInterface::StaticClass()))
	{
		IDamageableInterface::Execute_ReceiveDamage(OtherActor, Damage, GetOwner(), SweepResult.ImpactPoint);
	}
	Destroy();
}