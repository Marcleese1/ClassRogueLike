#include "Enemy/AttributeSet/EnemyAttributeSet.h"
#include "GameplayEffectExtension.h"
#include "Net/UnrealNetwork.h"
#include <BP_Enemy.h>

UEnemyAttributeSet::UEnemyAttributeSet()
{
    // No need for default values for Health and MaxHealth here
}

void UEnemyAttributeSet::PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data)
{
    Super::PostGameplayEffectExecute(Data);

    // If the health is being modified (damage or healing)
    if (Data.EvaluatedData.Attribute == GetHealthAttribute())
    {
        const float NewHealth = GetHealth();

        // Handle death if health falls to 0 or below
        if (NewHealth <= 0.0f)
        {
            AActor* Owner = GetOwningActor();
            if (ABP_Enemy* Enemy = Cast<ABP_Enemy>(Owner))
            {
                Enemy->Die();
            }
        }
    }
}

void UEnemyAttributeSet::OnRep_Health(const FGameplayAttributeData& OldHealth)
{
    GAMEPLAYATTRIBUTE_REPNOTIFY(UEnemyAttributeSet, Health, OldHealth);
}

void UEnemyAttributeSet::OnRep_MaxHealth(const FGameplayAttributeData& OldMaxHealth)
{
    GAMEPLAYATTRIBUTE_REPNOTIFY(UEnemyAttributeSet, MaxHealth, OldMaxHealth);
}

void UEnemyAttributeSet::OnRep_Armor(const FGameplayAttributeData& OldArmor)
{
    GAMEPLAYATTRIBUTE_REPNOTIFY(UEnemyAttributeSet, Armor, OldArmor);
}

void UEnemyAttributeSet::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    DOREPLIFETIME_CONDITION_NOTIFY(UEnemyAttributeSet, Health, COND_None, REPNOTIFY_Always);
    DOREPLIFETIME_CONDITION_NOTIFY(UEnemyAttributeSet, MaxHealth, COND_None, REPNOTIFY_Always);
    DOREPLIFETIME_CONDITION_NOTIFY(UEnemyAttributeSet, Armor, COND_None, REPNOTIFY_Always);
}
