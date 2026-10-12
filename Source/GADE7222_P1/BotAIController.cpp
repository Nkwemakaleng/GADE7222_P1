#include "BotAIController.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BaseCharacter.h"

ABotAIController::ABotAIController()
{
	PrimaryActorTick.bCanEverTick = false;
}

void ABotAIController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	if (BehaviorTreeAsset)
	{
		RunBehaviorTree(BehaviorTreeAsset);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("BotAIController: no BehaviorTreeAsset assigned!"));
	}
}