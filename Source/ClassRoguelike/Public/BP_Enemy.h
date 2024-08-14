#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CharacterBase.h"
#include "GameplayEffectTypes.h"
#include "BP_Enemy.generated.h"

class UCharacterAbilitySystemComponent;
class UCharacterAttributeSetBase;
class UBP_EnemyHealthBarWidget;
class UWidgetComponent;

UCLASS()
class CLASSROGUELIKE_API ABP_Enemy : public ACharacterBase
{
    GENERATED_BODY()

public:
    ABP_Enemy(const FObjectInitializer& ObjectInitializer);

    // Override the Tick function
    virtual void Tick(float DeltaTime) override;
    //UFUNCTION(BlueprintCallable, Category = "Enemy")
    virtual void Die() override;

protected:
    virtual void BeginPlay() override;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "UI")
    UWidgetComponent* UIFloatingStatusBarComponent;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "UI")
    TSubclassOf<UBP_EnemyHealthBarWidget> UIFloatingStatusBarClass;

    UPROPERTY(BlueprintReadOnly, Category = "UI")
    UBP_EnemyHealthBarWidget* UIFloatingStatusBar;

    virtual void OnHealthChanged(const FOnAttributeChangeData& Data);

    FDelegateHandle HealthChangedDelegateHandle;

    

    virtual void FinishDying() override;

    // New variables for rotation
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Movement")
    float RotationSpeed;

    FRotator TargetRotation;

    // Function to set the target rotation
    void UpdateTargetRotation();


    //// New function to safely handle enemy interaction
    //bool IsEnemyValid() const;
};
