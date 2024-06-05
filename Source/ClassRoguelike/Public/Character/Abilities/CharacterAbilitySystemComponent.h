// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "EnhancedInputComponent.h"  // Ensure this is included
#include "CharacterAbilitySystemComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FReceivedDamageDelegate, UCharacterAbilitySystemComponent*, SourceASC, float, UnmitigatedDamage, float, MitigatedDamage);

/**
 * UCharacterAbilitySystemComponent class
 */
UCLASS()
class CLASSROGUELIKE_API UCharacterAbilitySystemComponent : public UAbilitySystemComponent
{
	GENERATED_BODY()

public:
    bool CharacterAbilitiesGiven = false;
    bool StartupEffectsApplied = false;

    FReceivedDamageDelegate ReceivedDamage;

    virtual void OnGiveAbility(FGameplayAbilitySpec& AbilitySpec) override;
    virtual void ReceiveDamage(UCharacterAbilitySystemComponent* SourceASC, float UnmitigatedDamage, float MitigatedDamage);

    // Method to bind input actions to abilities
    void SetInputBinding(const UInputAction* InputAction, FGameplayAbilitySpecHandle AbilityHandle);

    // Method to manually process input actions
    void ProcessInputAction(const UInputAction* InputAction, bool bPressed);

private:
    bool bInputComponentInitialized;

    void InitializeInputComponent();
    UEnhancedInputComponent* InputComponent;

    // Delegate functions to handle input
    void OnAbilityInputPressed(FGameplayAbilitySpecHandle AbilityHandle);
    void OnAbilityInputReleased(FGameplayAbilitySpecHandle AbilityHandle);

    // Map to store input action to ability spec handle bindings
    TMap<const UInputAction*, FGameplayAbilitySpecHandle> InputToAbilityMap;
};