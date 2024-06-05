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

USTRUCT()
struct FAbilityInputBinding
{
    GENERATED_BODY()

    int32 InputID = 0;
    uint32 OnPressedHandle = 0;
    uint32 OnReleasedHandle = 0;
    TArray<FGameplayAbilitySpecHandle> BoundAbilitiesStack;
};


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

    // New functions for enhanced input abilities
    UFUNCTION(BlueprintCallable, Category = "Enhanced Input Abilities")
    void SetInputBinding(const UInputAction* InputAction, FGameplayAbilitySpecHandle AbilityHandle);


    UFUNCTION(BlueprintCallable, Category = "Enhanced Input Abilities")
    void ClearInputBinding(FGameplayAbilitySpecHandle AbilityHandle);

    UFUNCTION(BlueprintCallable, Category = "Enhanced Input Abilities")
    void ClearAbilityBindings(UInputAction* InputAction);

    // Method to manually process input actions
    void ProcessInputAction(const UInputAction* InputAction, bool bPressed);

private:
    bool bInputComponentInitialized;

    void InitializeInputComponent();
    UEnhancedInputComponent* InputComponent;

    // New private functions for handling input
    void OnAbilityInputPressed(UInputAction* InputAction);
    void OnAbilityInputReleased(UInputAction* InputAction);
    void RemoveEntry(UInputAction* InputAction);
    void TryBindAbilityInput(UInputAction* InputAction, FAbilityInputBinding& AbilityInputBinding);
    FGameplayAbilitySpec* FindAbilitySpec(FGameplayAbilitySpecHandle Handle);

    // New map to store input bindings
    TMap<UInputAction*, FAbilityInputBinding> MappedAbilities;

    // Map to store input action to ability spec handle bindings
    TMap<const UInputAction*, FGameplayAbilitySpecHandle> InputToAbilityMap;
};