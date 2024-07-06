#pragma once

#include "CoreMinimal.h"
#include "AttributeSet.h"
#include "AbilitySystemComponent.h"
#include "CharacterAttributeSetBase.generated.h"

// Uses macros from AttributeSet.h
#define ATTRIBUTE_ACCESSORS(ClassName, PropertyName) \
    GAMEPLAYATTRIBUTE_PROPERTY_GETTER(ClassName, PropertyName) \
    GAMEPLAYATTRIBUTE_VALUE_GETTER(PropertyName) \
    GAMEPLAYATTRIBUTE_VALUE_SETTER(PropertyName) \
    GAMEPLAYATTRIBUTE_VALUE_INITTER(PropertyName)

/**
 *
 */
UCLASS()
class CLASSROGUELIKE_API UCharacterAttributeSetBase : public UAttributeSet
{
    GENERATED_BODY()



public:

    // Constructor
    UCharacterAttributeSetBase();

    UPROPERTY(BlueprintReadOnly, Category = "Level", ReplicatedUsing = OnRep_Level)
    FGameplayAttributeData Level;
    ATTRIBUTE_ACCESSORS(UCharacterAttributeSetBase, Level)

        UPROPERTY(BlueprintReadOnly, Category = "Health", ReplicatedUsing = OnRep_Health)
    FGameplayAttributeData Health;
    ATTRIBUTE_ACCESSORS(UCharacterAttributeSetBase, Health)

        UPROPERTY(BlueprintReadOnly, Category = "Health", ReplicatedUsing = OnRep_MaxHealth)
    FGameplayAttributeData MaxHealth;
    ATTRIBUTE_ACCESSORS(UCharacterAttributeSetBase, MaxHealth)

        UPROPERTY(BlueprintReadOnly, Category = "Mana", ReplicatedUsing = OnRep_Mana)
    FGameplayAttributeData Mana;
    ATTRIBUTE_ACCESSORS(UCharacterAttributeSetBase, Mana)

        UPROPERTY(BlueprintReadOnly, Category = "Mana", ReplicatedUsing = OnRep_MaxMana)
    FGameplayAttributeData MaxMana;
    ATTRIBUTE_ACCESSORS(UCharacterAttributeSetBase, MaxMana)

        UPROPERTY(BlueprintReadOnly, Category = "Stamina", ReplicatedUsing = OnRep_Stamina)
    FGameplayAttributeData Stamina;
    ATTRIBUTE_ACCESSORS(UCharacterAttributeSetBase, Stamina)

        UPROPERTY(BlueprintReadOnly, Category = "Stamina", ReplicatedUsing = OnRep_MaxStamina)
    FGameplayAttributeData MaxStamina;
    ATTRIBUTE_ACCESSORS(UCharacterAttributeSetBase, MaxStamina)

        UPROPERTY(BlueprintReadOnly, Category = "Damage")
    FGameplayAttributeData Damage;
    ATTRIBUTE_ACCESSORS(UCharacterAttributeSetBase, Damage)

        // New attributes
        UPROPERTY(BlueprintReadOnly, Category = "Attributes", ReplicatedUsing = OnRep_DamageReduction)
    FGameplayAttributeData DamageReduction;
    ATTRIBUTE_ACCESSORS(UCharacterAttributeSetBase, DamageReduction)

        UPROPERTY(BlueprintReadOnly, Category = "Attributes", ReplicatedUsing = OnRep_CriticalHitDamage)
    FGameplayAttributeData CriticalHitDamage;
    ATTRIBUTE_ACCESSORS(UCharacterAttributeSetBase, CriticalHitDamage)

        UPROPERTY(BlueprintReadOnly, Category = "Attributes", ReplicatedUsing = OnRep_CriticalHitChance)
    FGameplayAttributeData CriticalHitChance;
    ATTRIBUTE_ACCESSORS(UCharacterAttributeSetBase, CriticalHitChance)

        UPROPERTY(BlueprintReadOnly, Category = "Attributes", ReplicatedUsing = OnRep_MovementSpeed)
    FGameplayAttributeData MovementSpeed;
    ATTRIBUTE_ACCESSORS(UCharacterAttributeSetBase, MovementSpeed)

        // Replication functions for the new attributes
        UFUNCTION()
    virtual void OnRep_DamageReduction(const FGameplayAttributeData& OldDamageReduction);
    UFUNCTION()
    virtual void OnRep_CriticalHitDamage(const FGameplayAttributeData& OldCriticalHitDamage);
    UFUNCTION()
    virtual void OnRep_CriticalHitChance(const FGameplayAttributeData& OldCriticalHitChance);
    UFUNCTION()
    virtual void OnRep_MovementSpeed(const FGameplayAttributeData& OldMovementSpeed);

    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

    // Existing replication functions
    UFUNCTION()
    virtual void OnRep_Level(const FGameplayAttributeData& OldLevel);
    UFUNCTION()
    virtual void OnRep_Health(const FGameplayAttributeData& OldHealth);
    UFUNCTION()
    virtual void OnRep_MaxHealth(const FGameplayAttributeData& OldMaxHealth);
    UFUNCTION()
    virtual void OnRep_Mana(const FGameplayAttributeData& OldMana);
    UFUNCTION()
    virtual void OnRep_MaxMana(const FGameplayAttributeData& OldMaxMana);
    UFUNCTION()
    virtual void OnRep_Stamina(const FGameplayAttributeData& OldStamina);
    UFUNCTION()
    virtual void OnRep_MaxStamina(const FGameplayAttributeData& OldMaxStamina);
};

//public:
//    UCharacterAttributeSetBase();
//
//    UPROPERTY(BlueprintReadOnly, Category = "Level", ReplicatedUsing = OnRep_Level)
//    FGameplayAttributeData Level;
//    ATTRIBUTE_ACCESSORS(UCharacterAttributeSetBase, Level)
//
//        UPROPERTY(BlueprintReadOnly, Category = "Health", ReplicatedUsing = OnRep_Health)
//    FGameplayAttributeData Health;
//    ATTRIBUTE_ACCESSORS(UCharacterAttributeSetBase, Health)
//
//        UPROPERTY(BlueprintReadOnly, Category = "Health", ReplicatedUsing = OnRep_MaxHealth)
//    FGameplayAttributeData MaxHealth;
//    ATTRIBUTE_ACCESSORS(UCharacterAttributeSetBase, MaxHealth)
//
//        UPROPERTY(BlueprintReadOnly, Category = "Mana", ReplicatedUsing = OnRep_Mana)
//    FGameplayAttributeData Mana;
//    ATTRIBUTE_ACCESSORS(UCharacterAttributeSetBase, Mana)
//
//        UPROPERTY(BlueprintReadOnly, Category = "Mana", ReplicatedUsing = OnRep_MaxMana)
//    FGameplayAttributeData MaxMana;
//    ATTRIBUTE_ACCESSORS(UCharacterAttributeSetBase, MaxMana)
//
//        UPROPERTY(BlueprintReadOnly, Category = "Stamina", ReplicatedUsing = OnRep_Stamina)
//    FGameplayAttributeData Stamina;
//    ATTRIBUTE_ACCESSORS(UCharacterAttributeSetBase, Stamina)
//
//        UPROPERTY(BlueprintReadOnly, Category = "Stamina", ReplicatedUsing = OnRep_MaxStamina)
//    FGameplayAttributeData MaxStamina;
//    ATTRIBUTE_ACCESSORS(UCharacterAttributeSetBase, MaxStamina)
//
//        UPROPERTY(BlueprintReadOnly, Category = "Damage")
//    FGameplayAttributeData Damage;
//    ATTRIBUTE_ACCESSORS(UCharacterAttributeSetBase, Damage)
//
//        UPROPERTY(BlueprintReadOnly, Category = "Attributes", ReplicatedUsing = OnRep_DamageReduction)
//    FGameplayAttributeData DamageReduction;
//    ATTRIBUTE_ACCESSORS(UCharacterAttributeSetBase, DamageReduction)
//
//        UPROPERTY(BlueprintReadOnly, Category = "Attributes", ReplicatedUsing = OnRep_CriticalHitDamage)
//    FGameplayAttributeData CriticalHitDamage;
//    ATTRIBUTE_ACCESSORS(UCharacterAttributeSetBase, CriticalHitDamage)
//
//        UPROPERTY(BlueprintReadOnly, Category = "Attributes", ReplicatedUsing = OnRep_CriticalHitChance)
//    FGameplayAttributeData CriticalHitChance;
//    ATTRIBUTE_ACCESSORS(UCharacterAttributeSetBase, CriticalHitChance)
//
//        UPROPERTY(BlueprintReadOnly, Category = "Attributes", ReplicatedUsing = OnRep_MovementSpeed)
//    FGameplayAttributeData MovementSpeed;
//    ATTRIBUTE_ACCESSORS(UCharacterAttributeSetBase, MovementSpeed)
//
//        virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
//
//    UFUNCTION()
//    virtual void OnRep_Level(const FGameplayAttributeData& OldLevel);
//    UFUNCTION()
//    virtual void OnRep_Health(const FGameplayAttributeData& OldHealth);
//    UFUNCTION()
//    virtual void OnRep_MaxHealth(const FGameplayAttributeData& OldMaxHealth);
//    UFUNCTION()
//    virtual void OnRep_Mana(const FGameplayAttributeData& OldMana);
//    UFUNCTION()
//    virtual void OnRep_MaxMana(const FGameplayAttributeData& OldMaxMana);
//    UFUNCTION()
//    virtual void OnRep_Stamina(const FGameplayAttributeData& OldStamina);
//    UFUNCTION()
//    virtual void OnRep_MaxStamina(const FGameplayAttributeData& OldMaxStamina);
//
//    UFUNCTION()
//    virtual void OnRep_DamageReduction(const FGameplayAttributeData& OldDamageReduction);
//    UFUNCTION()
//    virtual void OnRep_CriticalHitDamage(const FGameplayAttributeData& OldCriticalHitDamage);
//    UFUNCTION()
//    virtual void OnRep_CriticalHitChance(const FGameplayAttributeData& OldCriticalHitChance);
//    UFUNCTION()
//    virtual void OnRep_MovementSpeed(const FGameplayAttributeData& OldMovementSpeed);
//};