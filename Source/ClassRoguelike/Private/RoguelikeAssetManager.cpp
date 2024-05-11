// Fill out your copyright notice in the Description page of Project Settings.


#include "RoguelikeAssetManager.h"
#include "AbilitySystemGlobals.h"

void URoguelikeAssetManager :: StartInitialLoading()
{
	Super::StartInitialLoading();
	/*do this to log to check if asset manager in defaultengine.ini is in correctly*/
	UE_LOG(LogTemp, Warning, TEXT("Hello World"));
}