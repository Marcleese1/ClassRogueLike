// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "MainPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class CLASSROGUELIKE_API AMainPlayerController : public APlayerController
{
	GENERATED_BODY()

protected:
	virtual void OnPossess(APawn* InPawn) override;
};
