#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "InteractableInterface.generated.h"

UINTERFACE(MinimalAPI, Blueprintable)
class UInteractableInterface : public UInterface
{
	GENERATED_BODY()
};

class GADE7222_P1_API IInteractableInterface
{
	GENERATED_BODY()

public:
	// Called when the actor interacts with this object
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Interact")
	void Interact(AActor* Interactor);

	// Text shown in the HUD prompt"
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Interact")
	FText GetInteractPrompt() const;

	// Whether the actor can interact right now (false = greyed out / no prompt)
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Interact")
	bool CanInteract(AActor* Interactor) const;
};