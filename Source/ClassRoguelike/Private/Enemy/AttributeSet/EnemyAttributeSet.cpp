#include "Enemy/AttributeSet/EnemyAttributeSet.h"
#include "GameplayEffectExtension.h"
#include "Net/UnrealNetwork.h"
#include <BP_Enemy.h>

UEnemyAttributeSet::UEnemyAttributeSet()
{
    Health.SetBaseValue(100.0f);  // Set default health values
    Health.SetCurrentValue(100.0f);

    MaxHealth.SetBaseValue(100.0f);
    MaxHealth.SetCurrentValue(100.0f);

    Armor.SetBaseValue(0.0f);  // Set default armor values
    Armor.SetCurrentValue(0.0f);

    Damage.SetBaseValue(0.0f);  // Set default damage values
    Damage.SetCurrentValue(0.0f);

    MovementSpeed.SetBaseValue(600.0f);  // Set default movement speed values
    MovementSpeed.SetCurrentValue(600.0f);
}

void UEnemyAttributeSet::PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data)
{
    Super::PostGameplayEffectExecute(Data);

    // If we are dealing with damage or health changes
    if (Data.EvaluatedData.Attribute == GetDamageAttribute())
    {
        const float DamageDone = GetDamage();

        // Clamp new health
        const float NewHealth = FMath::Clamp(GetHealth() - DamageDone, 0.0f, GetMaxHealth());
        SetHealth(NewHealth);

        UE_LOG(LogTemp, Warning, TEXT("New Health After Damage: %f"), NewHealth);

        // Reset damage value
        SetDamage(0.0f);

        // Check if the entity is dead and handle death logic for enemies
        if (NewHealth <= 0.0f)
        {
            AActor* OwningActor = GetOwningActor();
            if (ABP_Enemy* Enemy = Cast<ABP_Enemy>(OwningActor))
            {
                Enemy->Die();  // Handle the enemy death within the enemy class
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

void UEnemyAttributeSet::OnRep_MovementSpeed(const FGameplayAttributeData& OldMovementSpeed)
{
    GAMEPLAYATTRIBUTE_REPNOTIFY(UEnemyAttributeSet, MovementSpeed, OldMovementSpeed);
}
void UEnemyAttributeSet::OnRep_Damage(const FGameplayAttributeData& OldDamage)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UEnemyAttributeSet, Damage, OldDamage);
}

void UEnemyAttributeSet::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    DOREPLIFETIME_CONDITION_NOTIFY(UEnemyAttributeSet, Health, COND_None, REPNOTIFY_Always);
    DOREPLIFETIME_CONDITION_NOTIFY(UEnemyAttributeSet, MaxHealth, COND_None, REPNOTIFY_Always);
    DOREPLIFETIME_CONDITION_NOTIFY(UEnemyAttributeSet, Armor, COND_None, REPNOTIFY_Always);
    DOREPLIFETIME_CONDITION_NOTIFY(UEnemyAttributeSet, Damage, COND_None, REPNOTIFY_Always);
    DOREPLIFETIME_CONDITION_NOTIFY(UEnemyAttributeSet, MovementSpeed, COND_None, REPNOTIFY_Always);
}
