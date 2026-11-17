// Fill out your copyright notice in the Description page of Project Settings.


#include "MyGameState.h"


AMyGameState::AMyGameState()
{
}

void AMyGameState::BroadcastKill(const FString& KillerName, const FString& VictimName, const FString& WeaponName)
{
	// Trigger the event for the UI
	OnKillFeedEvent.Broadcast(KillerName, VictimName, WeaponName);
}