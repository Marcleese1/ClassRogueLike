// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/Abilities/AttributeSets/AttributeExecutionCalculation.h"
#include "AbilitySystemComponent.h"
#include "GameplayEffect.h"
#include "GameplayEffectExtension.h"
#include "Character/Abilities/AttributeSets/CharacterAttributeSetBase.h"

UAttributeExecutionCalculation::UAttributeExecutionCalculation()
{
    RelevantAttributesToCapture.Add(FGameplayEffectAttributeCaptureDefinition(UCharacterAttributeSetBase::GetMaxHealthAttribute(), EGameplayEffectAttributeCaptureSource::Source, true));
}

void UAttributeExecutionCalculation::Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams, FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const
{
    UAbilitySystemComponent* TargetAbilitySystemComponent = ExecutionParams.GetTargetAbilitySystemComponent();

    if (TargetAbilitySystemComponent)
    {
        const FGameplayAttribute HealthAttribute = UCharacterAttributeSetBase::GetHealthAttribute();
        const FGameplayAttribute MaxHealthAttribute = UCharacterAttributeSetBase::GetMaxHealthAttribute();

        // Define capture definitions for the attributes
        FGameplayEffectAttributeCaptureDefinition MaxHealthCaptureDef(MaxHealthAttribute, EGameplayEffectAttributeCaptureSource::Source, true);
        FGameplayEffectAttributeCaptureDefinition HealthCaptureDef(HealthAttribute, EGameplayEffectAttributeCaptureSource::Target, false);

        FAggregatorEvaluateParameters EvaluationParameters;

        // Attempt to get the max health value from the target
        float MaxHealthValue = 0.0f;
        ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(MaxHealthCaptureDef, EvaluationParameters, MaxHealthValue);

        // Override the Health attribute with MaxHealth value
        OutExecutionOutput.AddOutputModifier(FGameplayModifierEvaluatedData(HealthAttribute, EGameplayModOp::Override, MaxHealthValue));
    }
}