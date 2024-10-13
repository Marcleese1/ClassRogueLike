#include "Enemy/EnemyAbilitySystemComponent.h"
#include "GameplayEffect.h"
#include "GameplayEffectExtension.h"
#include <BP_Enemy.h>

UEnemyAbilitySystemComponent::UEnemyAbilitySystemComponent()
{
        if (GetOwner() && GetOwner()->HasAuthority())
        {
        SetIsReplicated(true);
        SetReplicationMode(EGameplayEffectReplicationMode::Mixed);
        }
}

FActiveGameplayEffectHandle UEnemyAbilitySystemComponent::ApplyDefaultAttributesToEnemy(TSubclassOf<UGameplayEffect> DefaultAttributes, int32 Level)
{
    if (!DefaultAttributes)
    {
        UE_LOG(LogTemp, Error, TEXT("DefaultAttributes is null in UEnemyAbilitySystemComponent::ApplyDefaultAttributesToEnemy"));
        return FActiveGameplayEffectHandle();
    }

    FGameplayEffectContextHandle EffectContext = MakeEffectContext();
    EffectContext.AddSourceObject(this);

    FGameplayEffectSpecHandle SpecHandle = MakeOutgoingSpec(DefaultAttributes, Level, EffectContext);
    if (!SpecHandle.IsValid())
    {
        UE_LOG(LogTemp, Error, TEXT("Failed to create Spec for DefaultAttributes in UEnemyAbilitySystemComponent::ApplyDefaultAttributesToEnemy"));
        return FActiveGameplayEffectHandle();
    }


    return ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
}
