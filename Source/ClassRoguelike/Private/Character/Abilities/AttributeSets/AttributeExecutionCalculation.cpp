// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/Abilities/AttributeSets/AttributeExecutionCalculation.h"
#include "AbilitySystemComponent.h"
#include "GameplayEffect.h"
#include "GameplayEffectExtension.h"
#include "Character/Abilities/AttributeSets/CharacterAttributeSetBase.h"

UAttributeExecutionCalculation::UAttributeExecutionCalculation()
{
    // Define the attributes to capture
    RelevantAttributesToCapture.Add(FGameplayEffectAttributeCaptureDefinition(
        UCharacterAttributeSetBase::GetDamageAttribute(), EGameplayEffectAttributeCaptureSource::Source, true));
    RelevantAttributesToCapture.Add(FGameplayEffectAttributeCaptureDefinition(
        UCharacterAttributeSetBase::GetCriticalHitChanceAttribute(), EGameplayEffectAttributeCaptureSource::Source, true));
    RelevantAttributesToCapture.Add(FGameplayEffectAttributeCaptureDefinition(
        UCharacterAttributeSetBase::GetCriticalHitDamageAttribute(), EGameplayEffectAttributeCaptureSource::Source, true));
    RelevantAttributesToCapture.Add(FGameplayEffectAttributeCaptureDefinition(
        UCharacterAttributeSetBase::GetDamageReductionAttribute(), EGameplayEffectAttributeCaptureSource::Target, true));
}

void UAttributeExecutionCalculation::Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams, FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const
{
    UAbilitySystemComponent* SourceAbilitySystemComponent = ExecutionParams.GetSourceAbilitySystemComponent();
    UAbilitySystemComponent* TargetAbilitySystemComponent = ExecutionParams.GetTargetAbilitySystemComponent();

    if (SourceAbilitySystemComponent && TargetAbilitySystemComponent)
    {
        // Define attributes
        const FGameplayAttribute DamageAttribute = UCharacterAttributeSetBase::GetDamageAttribute();
        const FGameplayAttribute CriticalHitChanceAttribute = UCharacterAttributeSetBase::GetCriticalHitChanceAttribute();
        const FGameplayAttribute CriticalHitDamageAttribute = UCharacterAttributeSetBase::GetCriticalHitDamageAttribute();
        const FGameplayAttribute DamageReductionAttribute = UCharacterAttributeSetBase::GetDamageReductionAttribute();

        // Variables to hold the captured values
        float Damage = 0.0f;
        float CritChance = 0.0f;
        float CritDamage = 0.0f;
        float DamageReduction = 0.0f;

        FAggregatorEvaluateParameters EvaluationParameters;
        EvaluationParameters.SourceTags = ExecutionParams.GetOwningSpec().CapturedSourceTags.GetAggregatedTags();
        EvaluationParameters.TargetTags = ExecutionParams.GetOwningSpec().CapturedTargetTags.GetAggregatedTags();

        // Capture the attribute values
        ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(
            FGameplayEffectAttributeCaptureDefinition(DamageAttribute, EGameplayEffectAttributeCaptureSource::Source, true),
            EvaluationParameters, Damage);
        ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(
            FGameplayEffectAttributeCaptureDefinition(CriticalHitChanceAttribute, EGameplayEffectAttributeCaptureSource::Source, true),
            EvaluationParameters, CritChance);
        ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(
            FGameplayEffectAttributeCaptureDefinition(CriticalHitDamageAttribute, EGameplayEffectAttributeCaptureSource::Source, true),
            EvaluationParameters, CritDamage);
        ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(
            FGameplayEffectAttributeCaptureDefinition(DamageReductionAttribute, EGameplayEffectAttributeCaptureSource::Target, true),
            EvaluationParameters, DamageReduction);

        // Determine if the hit is critical
        bool bIsCriticalHit = FMath::FRand() <= CritChance;
        if (bIsCriticalHit)
        {
            Damage *= CritDamage;
        }

        // Apply damage reduction
        Damage *= (1.0f - DamageReduction);

        // Apply the damage to the target's health
        OutExecutionOutput.AddOutputModifier(FGameplayModifierEvaluatedData(UCharacterAttributeSetBase::GetHealthAttribute(), EGameplayModOp::Additive, -Damage));
    }
}