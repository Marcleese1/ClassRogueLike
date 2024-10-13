// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/Abilities/CharacterAbilitySystemComponent.h"
#include "EnemyAbilitySystemComponent.generated.h"

/**
 * 
 */
UCLASS()
class CLASSROGUELIKE_API UEnemyAbilitySystemComponent : public UCharacterAbilitySystemComponent
{
	GENERATED_BODY()

public:
    UEnemyAbilitySystemComponent();

    // Function to apply default effects for enemies
    FActiveGameplayEffectHandle ApplyDefaultAttributesToEnemy(TSubclassOf<UGameplayEffect> DefaultAttributes, int32 Level);
	
};
