// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "AbilitySystemInterface.h"
#include "GameplayTagContainer.h"
#include "GameplayAbilitySpec.h" 
#include "Character/Abilities/AttributeSets/CharacterAttributeSetBase.h"
#include "Enemy/AttributeSet/EnemyAttributeSet.h"
#include "Character/Abilities/CharacterAbilitySystemComponent.h"// Include for FGameplayAbilitySpecHandle
#include "InputAction.h" 
#include <ClassRoguelike/ClassRoguelike.h>
#include "CharacterBase.generated.h"



DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FCharacterDiedDelegate, ACharacterBase*, Character);


UCLASS()
class CLASSROGUELIKE_API ACharacterBase : public ACharacter, public IAbilitySystemInterface
{
	GENERATED_BODY()


public:
    // Sets default values for this character's properties
    ACharacterBase(const class FObjectInitializer& ObjectInitializer);

    // Default constructor
    ACharacterBase();

    virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;

    UPROPERTY(BlueprintAssignable, Category = "ClassRoguelike|Character")
    FCharacterDiedDelegate OnCharacterDied;

    UFUNCTION(BlueprintCallable, Category = "ClassRoguelike|Character")
    virtual bool IsAlive() const;

    UFUNCTION(BlueprintCallable, Category = "ClassRoguelike|Character")
    virtual int32 GetAbilityLevel(BaseAbilityID AbilityID) const;

    virtual void RemoveCharacterAbilities();
    virtual void Die();
    virtual void FinishDying();

    UFUNCTION(BlueprintCallable, Category = "ClassRoguelike|Character|Attributes")
    float GetCharacterLevel() const;

    UFUNCTION(BlueprintCallable, Category = "ClassRoguelike|Character|Attributes")
    float GetHealth() const;

    UFUNCTION(BlueprintCallable, Category = "ClassRoguelike|Character|Attributes")
    float GetMaxHealth() const;

    UFUNCTION(BlueprintCallable, Category = "ClassRoguelike|Character|Attributes")
    float GetMana() const;

    UFUNCTION(BlueprintCallable, Category = "ClassRoguelike|Character|Attributes")
    float GetMaxMana() const;

    UFUNCTION(BlueprintCallable, Category = "ClassRoguelike|Character|Attributes")
    float GetStamina() const;

    UFUNCTION(BlueprintCallable, Category = "ClassRoguelike|Character|Attributes")
    float GetMaxStamina() const;

protected:
    virtual void BeginPlay() override;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Abilities", meta = (AllowPrivateAccess = "true"))
    UCharacterAbilitySystemComponent* AbilitySystemComponent;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Abilities", meta = (AllowPrivateAccess = "true"))
    UCharacterAttributeSetBase* AttributeSetBase;
    
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Abilities", meta = (AllowPrivateAccess = "true"))
    UEnemyAttributeSet* EnemyAttributeSetBase;

    FGameplayTag DeadTag;
    FGameplayTag EffectRemoveOnDeathTag;

    UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "ClassRoguelike|Character")
    FText CharacterName;

    UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "ClassRoguelike|Animation")
    UAnimMontage* DeathMontage;

    UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "ClassRoguelike|Abilities")
    TMap<UInputAction*, TSubclassOf<class UCharacterGameplayAbility>> CharacterAbilities;

    UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "ClassRoguelike|Abilities")
    TSubclassOf<class UGameplayEffect> DefaultAttributes;

    UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "ClassRoguelike|Abilities")
    TArray<TSubclassOf<class UGameplayEffect>> StartupEffects;

    UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Abilities")
    TMap<UInputAction*, FGameplayAbilitySpecHandle> InputToAbilityMap;

    virtual void AddCharacterAbilities();
    virtual void InitializeAttributes();
    virtual void AddStartupEffects();
    virtual void SetHealth(float Health);
    virtual void SetMana(float Mana);
    virtual void SetStamina(float Stamina);
};