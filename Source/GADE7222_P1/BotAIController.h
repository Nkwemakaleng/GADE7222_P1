#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "BotAIController.generated.h"

UCLASS()
class GADE7222_P1_API ABotAIController : public AAIController
{
	GENERATED_BODY()

public:
	ABotAIController();

	UPROPERTY(EditDefaultsOnly, Category = "AI")
	class UBehaviorTree* BehaviorTreeAsset;

protected:
	virtual void OnPossess(APawn* InPawn) override;
};