// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/Abilities/AttributeSets/AttributeExecutionCalculation.h"
#include "AbilitySystemComponent.h"
#include "GameplayEffect.h"
#include "GameplayEffectExtension.h"
#include "Character/Abilities/AttributeSets/CharacterAttributeSetBase.h"

//UAttributeExecutionCalculation::UAttributeExecutionCalculation()
//{
//    // Define the capture for MaxHealth
//    RelevantAttributesToCapture.Add(FGameplayEffectAttributeCaptureDefinition(
//        UCharacterAttributeSetBase::GetMaxHealthAttribute(), EGameplayEffectAttributeCaptureSource::Source, true));
//}
//
//void UAttributeExecutionCalculation::Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams, FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const
//{
//    UAbilitySystemComponent* TargetAbilitySystemComponent = ExecutionParams.GetTargetAbilitySystemComponent();
//
//    if (TargetAbilitySystemComponent)
//    {
//        const FGameplayAttribute HealthAttribute = UCharacterAttributeSetBase::GetHealthAttribute();
//        const FGameplayAttribute MaxHealthAttribute = UCharacterAttributeSetBase::GetMaxHealthAttribute();
//
//        float MaxHealthValue = 0.0f;
//        ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(MaxHealthAttribute, FAggregatorEvaluateParameters(), MaxHealthValue);
//
//        OutExecutionOutput.AddOutputModifier(FGameplayModifierEvaluatedData(HealthAttribute, EGameplayModOp::Override, MaxHealthValue));
//    }
//}