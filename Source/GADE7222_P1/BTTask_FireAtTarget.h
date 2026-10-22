#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BBKeys.h"
#include "BTTask_FireAtTarget.generated.h"

UCLASS()
class GADE7222_P1_API UBTTask_FireAtTarget : public UBTTaskNode
{
	GENERATED_BODY()

public:
	UBTTask_FireAtTarget();

	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	virtual void TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;
	virtual EBTNodeResult::Type AbortTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

	// --- COMBAT DISTANCE ---
	UPROPERTY(EditAnywhere, Category = "Bot|Combat")
	float MinFireDistance = 300.f;

	UPROPERTY(EditAnywhere, Category = "Bot|Combat")
	float MaxFireDistance = 1200.f;

	UPROPERTY(EditAnywhere, Category = "Bot|Combat")
	float MaxEngageDuration = 8.0f;

	// --- HUMAN FACTORS ---
	UPROPERTY(EditAnywhere, Category = "Bot|Human")
	float MinReactionTime = 0.2f;

	UPROPERTY(EditAnywhere, Category = "Bot|Human")
	float MaxReactionTime = 0.5f;

	UPROPERTY(EditAnywhere, Category = "Bot|Human")
	float AimErrorDegrees = 3.0f; // How much they can miss by

	UPROPERTY(EditAnywhere, Category = "Bot|Human")
	float JumpChancePerTick = 0.02f; // 2% chance to jump each tick

	// --- MOVEMENT ---
	UPROPERTY(EditAnywhere, Category = "Bot|Movement")
	float StrafeChangeMinTime = 0.5f;

	UPROPERTY(EditAnywhere, Category = "Bot|Movement")
	float StrafeChangeMaxTime = 1.5f;

private:
	AActor* GetTarget(UBehaviorTreeComponent& OwnerComp) const;
	float AccumulatedTime = 0.0f;
	float CurrentReactionTime = 0.0f;
	bool bIsReadyToFire = false;
	
	float StrafeTimer = 0.0f;
	float StrafeDirection = 1.0f; // 1.0 = Right, -1.0 = Left
};