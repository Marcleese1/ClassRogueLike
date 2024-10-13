#pragma once

#include "CoreMinimal.h"
#include "CharacterBase.h"
#include "Enemy/UI/BP_EnemyHealthBarWidget.h"
#include "Components/WidgetComponent.h"
#include "Character/Abilities/CharacterAbilitySystemComponent.h"
#include "Enemy/AttributeSet/EnemyAttributeSet.h"
#include "Enemy/EnemyAbilitySystemComponent.h"
#include "BP_Enemy.generated.h"

UCLASS()
class CLASSROGUELIKE_API ABP_Enemy : public ACharacterBase
{
    GENERATED_BODY()

public:
    ABP_Enemy(const FObjectInitializer& ObjectInitializer);

    // Getters
    UAbilitySystemComponent* GetAbilitySystemComponent() const override;
    UEnemyAttributeSet* GetAttributeSetBase() const; // Updated to UEnemyAttributeSet
    virtual void Die();

    void UpdateTargetRotation();
    void FinishDying();
    void SetHealth(float HealthValue) override;

    float GetMaxHealth() const;
    float GetHealth() const;

protected:
    virtual void BeginPlay() override;
    void Tick(float DeltaTime) override;  // Added override for tick function.

    // Ability System Component and Attribute Set
    UPROPERTY(BlueprintReadOnly)
    UEnemyAbilitySystemComponent* EnemyAbilitySystemComponent;

    UPROPERTY(BlueprintReadOnly)
    UEnemyAttributeSet* EnemyAttributeSet;

    // Attribute Gameplay Effect
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Abilities")
    TSubclassOf<UGameplayEffect> DefaultAttributesForEnemies;

    // UI Health Bar
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "UI")
    UWidgetComponent* UIFloatingStatusBarComponent;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attributes")
    float InitialHealth;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attributes")
    float InitialMaxHealth;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "UI")
    TSubclassOf<UBP_EnemyHealthBarWidget> UIFloatingStatusBarClass;

    UPROPERTY(BlueprintReadOnly, Category = "UI")
    UBP_EnemyHealthBarWidget* UIFloatingStatusBar;

    // Rotation variables
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Movement")
    float RotationSpeed;

    FRotator TargetRotation;

    FDelegateHandle HealthChangedDelegateHandle;
    FDelegateHandle MaxHealthChangedDelegateHandle;
    FGameplayTag DeadTag;

    // Functions
    void InitializeAttributes();
    void AddStartupEffects();
    void UpdateHealthBar();
    void OnHealthChanged(const FOnAttributeChangeData& Data);
    void OnMaxHealthChangedDelegate(const FOnAttributeChangeData& Data);
};
