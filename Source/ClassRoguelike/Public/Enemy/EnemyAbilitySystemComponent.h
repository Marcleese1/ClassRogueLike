#pragma once

#include "CoreMinimal.h"
#include "Character/Abilities/CharacterAbilitySystemComponent.h"
#include "EnemyAbilitySystemComponent.generated.h"

UCLASS()
class CLASSROGUELIKE_API UEnemyAbilitySystemComponent : public UCharacterAbilitySystemComponent
{
    GENERATED_BODY()

public:
    UEnemyAbilitySystemComponent();

    FActiveGameplayEffectHandle ApplyDefaultAttributesToEnemy(TSubclassOf<UGameplayEffect> DefaultAttributes, int32 Level);
};
