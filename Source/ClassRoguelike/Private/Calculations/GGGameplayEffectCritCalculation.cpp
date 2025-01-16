#include "Calculations/GGGameplayEffectCritCalculation.h"
#include "Character/Abilities/AttributeSets/CharacterAttributeSetBase.h"
#include "Enemy/AttributeSet/EnemyAttributeSet.h"
#include "Engine/Engine.h"
#include "AbilitySystemBlueprintLibrary.h" // Required for accessing the Ability System Component

UGGGameplayEffectCritCalculation::UGGGameplayEffectCritCalculation()
{
    // Capture Critical Hit Chance from Source
    RelevantAttributesToCapture.Add(FGameplayEffectAttributeCaptureDefinition(
        UCharacterAttributeSetBase::GetCriticalHitChanceAttribute(),
        EGameplayEffectAttributeCaptureSource::Source, true));

    // Capture Critical Hit Damage from Source
    RelevantAttributesToCapture.Add(FGameplayEffectAttributeCaptureDefinition(
        UCharacterAttributeSetBase::GetCriticalHitDamageAttribute(),
        EGameplayEffectAttributeCaptureSource::Source, true));

    // Capture Damage from Target
    RelevantAttributesToCapture.Add(FGameplayEffectAttributeCaptureDefinition(
        UEnemyAttributeSet::GetDamageAttribute(),
        EGameplayEffectAttributeCaptureSource::Target, true));
}

void UGGGameplayEffectCritCalculation::Execute_Implementation(
    const FGameplayEffectCustomExecutionParameters& ExecutionParams,
    FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const
{
    float CriticalHitChance = 0.f;
    float CriticalHitDamage = 0.f;
    float BaseDamage = 0.f;
    float FinalDamage = 0.f;

    // --- Fix: Get the Target Actor's Damage Attribute Instance ---
    const FGameplayEffectSpec& Spec = ExecutionParams.GetOwningSpec();
    AActor* TargetActor = Spec.GetContext().GetEffectCauser();

    if (TargetActor)
    {
        UAbilitySystemComponent* TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(TargetActor);
        if (TargetASC)
        {
            // Cast to `const UEnemyAttributeSet*` and then remove `const` for modification
            const UEnemyAttributeSet* EnemyAttributesConst = TargetASC->GetSet<UEnemyAttributeSet>();
            if (EnemyAttributesConst)
            {
                UEnemyAttributeSet* EnemyAttributes = const_cast<UEnemyAttributeSet*>(EnemyAttributesConst);
                UE_LOG(LogTemp, Warning, TEXT("Pre-Capture Enemy Damage: %f"), EnemyAttributes->GetDamage());
            }
        }
    }
    // --- End of Fix ---

    // Attempt to capture Critical Hit Chance
    if (ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(
            RelevantAttributesToCapture[0],  // Using the first defined capture (CriticalHitChance)
            FAggregatorEvaluateParameters(), CriticalHitChance))
    {
        UE_LOG(LogTemp, Warning, TEXT("Captured Critical Hit Chance: %f"), CriticalHitChance);
    }

    // Attempt to capture Critical Hit Damage
    if (ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(
            RelevantAttributesToCapture[1],  // Using the second defined capture (CriticalHitDamage)
            FAggregatorEvaluateParameters(), CriticalHitDamage))
    {
        UE_LOG(LogTemp, Warning, TEXT("Captured Critical Hit Damage: %f"), CriticalHitDamage);
    }

    // Attempt to capture Base Damage
    if (ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(
            RelevantAttributesToCapture[2],  // Using the third defined capture (BaseDamage)
            FAggregatorEvaluateParameters(), BaseDamage))
    {
        UE_LOG(LogTemp, Warning, TEXT("Captured Base Damage: %f"), BaseDamage);
    }

    // Log values to the screen for an extended duration
    if (GEngine)
    {
        for (int32 i = 0; i < 60; ++i) // Approximately 60 frames of log visibility
        {
            GEngine->AddOnScreenDebugMessage(-1, 60.f, FColor::Yellow,
                                             FString::Printf(TEXT("Crit Chance: %f"), CriticalHitChance));
            GEngine->AddOnScreenDebugMessage(-1, 60.f, FColor::Yellow,
                                             FString::Printf(TEXT("Crit Damage: %f"), CriticalHitDamage));
            GEngine->AddOnScreenDebugMessage(-1, 60.f, FColor::Yellow,
                                             FString::Printf(TEXT("Base Damage: %f"), BaseDamage));
        }
    }

    // Clamp Critical Hit Chance
    CriticalHitChance = FMath::Clamp(CriticalHitChance, 0.f, 1.f);

    // Determine if a critical hit occurs
    const float RandomValue = FMath::RandRange(0.f, 1.f);
    if (RandomValue <= CriticalHitChance)
    {
        // Critical hit
        FinalDamage = BaseDamage * CriticalHitDamage;
        UE_LOG(LogTemp, Warning, TEXT("Critical Hit! Final Damage: %f"), FinalDamage);

        if (GEngine)
        {
            GEngine->AddOnScreenDebugMessage(-1, 60.f, FColor::Red,
                                             FString::Printf(TEXT("Critical Hit! Damage: %f"), FinalDamage));
        }
    }
    else
    {
        // Normal hit
        FinalDamage = BaseDamage;
        UE_LOG(LogTemp, Warning, TEXT("Normal Hit. Final Damage: %f"), FinalDamage);

        if (GEngine)
        {
            GEngine->AddOnScreenDebugMessage(-1, 60.f, FColor::White,
                                             FString::Printf(TEXT("Normal Hit. Damage: %f"), FinalDamage));
        }
    }

    // Apply the final damage
    OutExecutionOutput.AddOutputModifier(FGameplayModifierEvaluatedData(
        UEnemyAttributeSet::GetDamageAttribute(), EGameplayModOp::Additive, FinalDamage));

    // Log final damage applied
    UE_LOG(LogTemp, Warning, TEXT("Damage Applied to Enemy: %f"), FinalDamage);

    if (GEngine)
    {
        GEngine->AddOnScreenDebugMessage(-1, 60.f, FColor::Blue,
                                         FString::Printf(TEXT("Damage Applied to Enemy: %f"), FinalDamage));
    }
}
