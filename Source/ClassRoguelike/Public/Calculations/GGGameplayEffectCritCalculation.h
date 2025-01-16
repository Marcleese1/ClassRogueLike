#pragma once

#include "CoreMinimal.h"
#include "GameplayEffectExecutionCalculation.h"
#include "GGGameplayEffectCritCalculation.generated.h"

/**
 * Execution Calculation for handling critical hits.
 */
UCLASS()
class CLASSROGUELIKE_API UGGGameplayEffectCritCalculation : public UGameplayEffectExecutionCalculation
{
	GENERATED_BODY()

public:
	// Constructor
	UGGGameplayEffectCritCalculation();

	// Main execution logic
	virtual void Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams,
										FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const override;
};
