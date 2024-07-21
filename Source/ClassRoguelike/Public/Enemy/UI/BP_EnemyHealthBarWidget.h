#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "BP_EnemyHealthBarWidget.generated.h"

UCLASS()
class CLASSROGUELIKE_API UBP_EnemyHealthBarWidget : public UUserWidget
{
    GENERATED_BODY()

public:
    UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
    void SetHealthPercentage(float HealthPercentage);
};
