#include "AmmoPickup.h"
#include "Components/SphereComponent.h"
#include "BaseCharacter.h" // Cast to BaseCharacter, not PlayerCharacter!

AAmmoPickup::AAmmoPickup()
{
	PrimaryActorTick.bCanEverTick = false;

	PickupSphere = CreateDefaultSubobject<USphereComponent>(TEXT("PickupSphere"));
	PickupSphere->InitSphereRadius(50.0f);
	PickupSphere->SetCollisionProfileName(TEXT("OverlapAllDynamic"));
	RootComponent = PickupSphere;

	PickupMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("PickupMesh"));
	PickupMesh->SetupAttachment(RootComponent);
	PickupMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	PickupSphere->OnComponentBeginOverlap.AddDynamic(this, &AAmmoPickup::OnOverlap);
}

void AAmmoPickup::BeginPlay()
{
	Super::BeginPlay();
}

void AAmmoPickup::OnOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult)
{
	if (ABaseCharacter* BaseChar = Cast<ABaseCharacter>(OtherActor))
	{
		BaseChar->PickupWeapon(WeaponToPickup, AmmoAmount);
		Destroy();
	}
}