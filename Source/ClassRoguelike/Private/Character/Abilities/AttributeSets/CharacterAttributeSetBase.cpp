#include "Character/Abilities/AttributeSets/CharacterAttributeSetBase.h"
#include "Net/UnrealNetwork.h"
#include "GameFramework/Actor.h"
#include "GameplayEffect.h"
#include "GameplayEffectExtension.h"
#include "BP_Enemy.h"
#include "CharacterBase.h" // Ensure this is included to reference player-specific logic
#include <Character/Player/MainPlayerCharacter.h>

UCharacterAttributeSetBase::UCharacterAttributeSetBase()
{
    Level.SetBaseValue(1.0f);
    Level.SetCurrentValue(1.0f);

    Health.SetBaseValue(100.0f);
    Health.SetCurrentValue(100.0f);

    MaxHealth.SetBaseValue(100.0f);
    MaxHealth.SetCurrentValue(100.0f);

    Mana.SetBaseValue(100.0f);
    Mana.SetCurrentValue(100.0f);

    MaxMana.SetBaseValue(100.0f);
    MaxMana.SetCurrentValue(100.0f);

    Stamina.SetBaseValue(100.0f);
    Stamina.SetCurrentValue(100.0f);

    MaxStamina.SetBaseValue(100.0f);
    MaxStamina.SetCurrentValue(100.0f);

    Damage.SetBaseValue(10.0f);
    Damage.SetCurrentValue(10.0f);

    DamageReduction.SetBaseValue(0.0f);
    DamageReduction.SetCurrentValue(0.0f);

    CriticalHitDamage.SetBaseValue(2.0f);
    CriticalHitDamage.SetCurrentValue(2.0f);

    CriticalHitChance.SetBaseValue(0.1f);
    CriticalHitChance.SetCurrentValue(0.1f);

    MovementSpeed.SetBaseValue(600.0f);
    MovementSpeed.SetCurrentValue(600.0f);
}

void UCharacterAttributeSetBase::PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data)
{
    Super::PostGameplayEffectExecute(Data);

    if (Data.EvaluatedData.Attribute == GetDamageAttribute())
    {
        // Decrease health by the damage amount
        const float DamageDone = GetDamage();
        SetHealth(FMath::Clamp(GetHealth() - DamageDone, 0.0f, GetMaxHealth()));

        // Reset Damage attribute
        SetDamage(0.0f);

        // Check if health reached zero
        AActor* OwningActor = GetOwningActor();
        if (GetHealth() <= 0)
        {
            if (ABP_Enemy* Enemy = Cast<ABP_Enemy>(OwningActor))
            {
                Enemy->Die();
            }
            else if (AMainPlayerCharacter* Player = Cast<AMainPlayerCharacter>(OwningActor))
            {
                Player->Die();
            }
            // Handle other cases or actors if needed in the future
        }
    }
}


// The rest of the replication functions and attribute changes notifications remain the same

void UCharacterAttributeSetBase::OnRep_Level(const FGameplayAttributeData& OldLevel)
{
    GAMEPLAYATTRIBUTE_REPNOTIFY(UCharacterAttributeSetBase, Level, OldLevel);
}

void UCharacterAttributeSetBase::OnRep_Health(const FGameplayAttributeData& OldHealth)
{
    GAMEPLAYATTRIBUTE_REPNOTIFY(UCharacterAttributeSetBase, Health, OldHealth);
}

void UCharacterAttributeSetBase::OnRep_MaxHealth(const FGameplayAttributeData& OldMaxHealth)
{
    GAMEPLAYATTRIBUTE_REPNOTIFY(UCharacterAttributeSetBase, MaxHealth, OldMaxHealth);
}

void UCharacterAttributeSetBase::OnRep_Mana(const FGameplayAttributeData& OldMana)
{
    GAMEPLAYATTRIBUTE_REPNOTIFY(UCharacterAttributeSetBase, Mana, OldMana);
}

void UCharacterAttributeSetBase::OnRep_MaxMana(const FGameplayAttributeData& OldMaxMana)
{
    GAMEPLAYATTRIBUTE_REPNOTIFY(UCharacterAttributeSetBase, MaxMana, OldMaxMana);
}

void UCharacterAttributeSetBase::OnRep_Stamina(const FGameplayAttributeData& OldStamina)
{
    GAMEPLAYATTRIBUTE_REPNOTIFY(UCharacterAttributeSetBase, Stamina, OldStamina);
}

void UCharacterAttributeSetBase::OnRep_MaxStamina(const FGameplayAttributeData& OldMaxStamina)
{
    GAMEPLAYATTRIBUTE_REPNOTIFY(UCharacterAttributeSetBase, MaxStamina, OldMaxStamina);
}

void UCharacterAttributeSetBase::OnRep_DamageReduction(const FGameplayAttributeData& OldDamageReduction)
{
    GAMEPLAYATTRIBUTE_REPNOTIFY(UCharacterAttributeSetBase, DamageReduction, OldDamageReduction);
}

void UCharacterAttributeSetBase::OnRep_CriticalHitDamage(const FGameplayAttributeData& OldCriticalHitDamage)
{
    GAMEPLAYATTRIBUTE_REPNOTIFY(UCharacterAttributeSetBase, CriticalHitDamage, OldCriticalHitDamage);
}

void UCharacterAttributeSetBase::OnRep_CriticalHitChance(const FGameplayAttributeData& OldCriticalHitChance)
{
    GAMEPLAYATTRIBUTE_REPNOTIFY(UCharacterAttributeSetBase, CriticalHitChance, OldCriticalHitChance);
}

void UCharacterAttributeSetBase::OnRep_MovementSpeed(const FGameplayAttributeData& OldMovementSpeed)
{
    GAMEPLAYATTRIBUTE_REPNOTIFY(UCharacterAttributeSetBase, MovementSpeed, OldMovementSpeed);
}

void UCharacterAttributeSetBase::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    DOREPLIFETIME_CONDITION_NOTIFY(UCharacterAttributeSetBase, Level, COND_None, REPNOTIFY_Always);
    DOREPLIFETIME_CONDITION_NOTIFY(UCharacterAttributeSetBase, Health, COND_None, REPNOTIFY_Always);
    DOREPLIFETIME_CONDITION_NOTIFY(UCharacterAttributeSetBase, MaxHealth, COND_None, REPNOTIFY_Always);
    DOREPLIFETIME_CONDITION_NOTIFY(UCharacterAttributeSetBase, Mana, COND_None, REPNOTIFY_Always);
    DOREPLIFETIME_CONDITION_NOTIFY(UCharacterAttributeSetBase, MaxMana, COND_None, REPNOTIFY_Always);
    DOREPLIFETIME_CONDITION_NOTIFY(UCharacterAttributeSetBase, Stamina, COND_None, REPNOTIFY_Always);
    DOREPLIFETIME_CONDITION_NOTIFY(UCharacterAttributeSetBase, MaxStamina, COND_None, REPNOTIFY_Always);
    DOREPLIFETIME_CONDITION_NOTIFY(UCharacterAttributeSetBase, DamageReduction, COND_None, REPNOTIFY_Always);
    DOREPLIFETIME_CONDITION_NOTIFY(UCharacterAttributeSetBase, CriticalHitDamage, COND_None, REPNOTIFY_Always);
    DOREPLIFETIME_CONDITION_NOTIFY(UCharacterAttributeSetBase, CriticalHitChance, COND_None, REPNOTIFY_Always);
    DOREPLIFETIME_CONDITION_NOTIFY(UCharacterAttributeSetBase, MovementSpeed, COND_None, REPNOTIFY_Always);
}

void UCharacterAttributeSetBase::LogAttributeValues() const
{
    UE_LOG(LogTemp, Log, TEXT("Logging Attribute Values for %s"), *GetOwningActor()->GetName());
    UE_LOG(LogTemp, Log, TEXT("Level: %f"), Level.GetCurrentValue());
    UE_LOG(LogTemp, Log, TEXT("Health: %f"), Health.GetCurrentValue());
    UE_LOG(LogTemp, Log, TEXT("MaxHealth: %f"), MaxHealth.GetCurrentValue());
    UE_LOG(LogTemp, Log, TEXT("Mana: %f"), Mana.GetCurrentValue());
    UE_LOG(LogTemp, Log, TEXT("MaxMana: %f"), MaxMana.GetCurrentValue());
    UE_LOG(LogTemp, Log, TEXT("Stamina: %f"), Stamina.GetCurrentValue());
    UE_LOG(LogTemp, Log, TEXT("MaxStamina: %f"), MaxStamina.GetCurrentValue());
    UE_LOG(LogTemp, Log, TEXT("Damage: %f"), Damage.GetCurrentValue());
    UE_LOG(LogTemp, Log, TEXT("DamageReduction: %f"), DamageReduction.GetCurrentValue());
    UE_LOG(LogTemp, Log, TEXT("CriticalHitDamage: %f"), CriticalHitDamage.GetCurrentValue());
    UE_LOG(LogTemp, Log, TEXT("CriticalHitChance: %f"), CriticalHitChance.GetCurrentValue());
    UE_LOG(LogTemp, Log, TEXT("MovementSpeed: %f"), MovementSpeed.GetCurrentValue());
}
