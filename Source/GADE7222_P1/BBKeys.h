#pragma once

#include "CoreMinimal.h"

// Centralised Blackboard key names. Single source of truth so C++ and BT editor never drift.
namespace BotBBKeys
{
	static const FName TargetActor      (TEXT("TargetActor"));
	static const FName HasLineOfSight   (TEXT("HasLineOfSight"));
	static const FName HealthPickup     (TEXT("HealthPickup"));
	static const FName AmmoPickup       (TEXT("AmmoPickup"));
	static const FName PatrolPoint      (TEXT("PatrolPoint"));
	static const FName CurrentIntent    (TEXT("CurrentIntent"));
}

UENUM(BlueprintType)
enum class EBotIntent : uint8
{
	Patrol      UMETA(DisplayName = "Patrol"),
	Engage      UMETA(DisplayName = "Engage"),
	SeekHealth  UMETA(DisplayName = "Seek Health"),
	SeekAmmo    UMETA(DisplayName = "Seek Ammo"),
	Flee        UMETA(DisplayName = "Flee")
};