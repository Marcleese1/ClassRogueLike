#include "Character/Abilities/AttributeSets/CharacterAttributeSetBase.h"
#include "Net/UnrealNetwork.h"

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
    GAMEPLAYATTRIBUTE_REPNOTIFY(UCharacterAttributeSetBase, Mana, OldStamina);
}

void UCharacterAttributeSetBase::OnRep_MaxStamina(const FGameplayAttributeData& OldMaxStamina)
{
    GAMEPLAYATTRIBUTE_REPNOTIFY(UCharacterAttributeSetBase, MaxMana, OldMaxStamina);
}

// Implement the replication functions for the new attributes
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

    // Add replication for the new attributes
    DOREPLIFETIME_CONDITION_NOTIFY(UCharacterAttributeSetBase, DamageReduction, COND_None, REPNOTIFY_Always);
    DOREPLIFETIME_CONDITION_NOTIFY(UCharacterAttributeSetBase, CriticalHitDamage, COND_None, REPNOTIFY_Always);
    DOREPLIFETIME_CONDITION_NOTIFY(UCharacterAttributeSetBase, CriticalHitChance, COND_None, REPNOTIFY_Always);
    DOREPLIFETIME_CONDITION_NOTIFY(UCharacterAttributeSetBase, MovementSpeed, COND_None, REPNOTIFY_Always);
}
