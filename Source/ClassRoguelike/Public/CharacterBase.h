// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "AbilitySystemInterface.h"
#include "GameplayTagContainer.h"
#include "GameplayAbilitySpec.h"  // Include for FGameplayAbilitySpecHandle
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

    // This needs to be implemented when using IAbilitySystemInterface
    virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;

    UPROPERTY(BlueprintAssignable, Category = "ClassRoguelike|Character")
    FCharacterDiedDelegate OnCharacterDied;

    UFUNCTION(BlueprintCallable, Category = "ClassRoguelike|Character")
    virtual bool IsAlive() const;

    UFUNCTION(BlueprintCallable, Category = "ClassRoguelike|Character")
    virtual int32 GetAbilityLevel(BaseAbilityID AbilityID) const;

    virtual void RemoveCharacterAbilities();

    virtual void Die();

    UFUNCTION(BlueprintCallable, Category = "ClassRoguelike|Character")
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

protected:
    // Called when the game starts or when spawned
    virtual void BeginPlay() override;

    TWeakObjectPtr<class UCharacterAbilitySystemComponent> AbilitySystemComponent;
    TWeakObjectPtr<class UCharacterAttributeSetBase> AttributeSetBase;

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
};