#include "EnemyCharacter.h"

#include "BotAIController.h"
#include "Components/SkeletalMeshComponent.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Materials/MaterialInterface.h"

AEnemyCharacter::AEnemyCharacter()
{
	// AI does not need a camera or spring arm — leave them, they don't hurt
	AIControllerClass = ABotAIController::StaticClass();
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;
}

void AEnemyCharacter::BeginPlay()
{
	Super::BeginPlay();
	ApplyBotColor();
}

void AEnemyCharacter::ApplyBotColor()
{
	if (!GetMesh()) return;

	UMaterialInterface* BaseMat = GetMesh()->GetMaterial(0);
	if (!BaseMat) return;

	BodyMID = UMaterialInstanceDynamic::Create(BaseMat, this);
	if (BodyMID)
	{
		BodyMID->SetVectorParameterValue(TEXT("Color"), BotColor);
		BodyMID->SetVectorParameterValue(TEXT("BaseColor"), BotColor);
		GetMesh()->SetMaterial(0, BodyMID);
	}
}