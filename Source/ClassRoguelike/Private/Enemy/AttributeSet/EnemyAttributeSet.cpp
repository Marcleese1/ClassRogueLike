#include "Enemy/AttributeSet/EnemyAttributeSet.h"
#include "GameplayEffectExtension.h"
#include "Net/UnrealNetwork.h"
#include <BP_Enemy.h>

UEnemyAttributeSet::UEnemyAttributeSet()
{
    // Initialize default values for attributes
    Health.SetBaseValue(100.0f);
    Health.SetCurrentValue(100.0f);

    MaxHealth.SetBaseValue(100.0f);
    MaxHealth.SetCurrentValue(100.0f);

    Armor.SetBaseValue(50.0f);
    Armor.SetCurrentValue(50.0f);
}


void UEnemyAttributeSet::PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data)
{
    Super::PostGameplayEffectExecute(Data);

    // Check if the current effect is dealing with damage
    if (Data.EvaluatedData.Attribute == GetHealthAttribute())
    {
        // Damage can be either healing (negative value) or actual damage (positive value)
        const float Damage = GetHealth(); // Get the current health, which may have changed due to the effect

        if (Damage > 0.0f)
        {
            // Apply damage (subtract from health)
            const float NewHealth = FMath::Clamp(GetHealth() - Damage, 0.0f, GetMaxHealth());
            SetHealth(NewHealth);

            // Log for debugging
            UE_LOG(LogTemp, Warning, TEXT("Enemy took damage. New Health: %f"), NewHealth);

            // Check for death condition
            if (NewHealth <= 0.0f)
            {
                // Handle death (replace with your custom death handling)
                AActor* Owner = GetOwningActor();
                if (ABP_Enemy* Enemy = Cast<ABP_Enemy>(Owner))
                {
                    Enemy->Die();
                }
            }
        }

        // Clear the damage attribute after applying it
        SetHealth(0.0f);
    }
}



// Replication notification function for Health
// Replication notification function for Health
void UEnemyAttributeSet::OnRep_Health(const FGameplayAttributeData& OldHealth)
{
    GAMEPLAYATTRIBUTE_REPNOTIFY(UEnemyAttributeSet, Health, OldHealth);
}

// Replication notification function for MaxHealth
void UEnemyAttributeSet::OnRep_MaxHealth(const FGameplayAttributeData& OldMaxHealth)
{
    GAMEPLAYATTRIBUTE_REPNOTIFY(UEnemyAttributeSet, MaxHealth, OldMaxHealth);
}

// Replication notification function for Armor
void UEnemyAttributeSet::OnRep_Armor(const FGameplayAttributeData& OldArmor)
{
    GAMEPLAYATTRIBUTE_REPNOTIFY(UEnemyAttributeSet, Armor, OldArmor);
}


// Replication support
void UEnemyAttributeSet::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    DOREPLIFETIME_CONDITION_NOTIFY(UEnemyAttributeSet, Health, COND_None, REPNOTIFY_Always);
    DOREPLIFETIME_CONDITION_NOTIFY(UEnemyAttributeSet, MaxHealth, COND_None, REPNOTIFY_Always);
    DOREPLIFETIME_CONDITION_NOTIFY(UEnemyAttributeSet, Armor, COND_None, REPNOTIFY_Always);
}

