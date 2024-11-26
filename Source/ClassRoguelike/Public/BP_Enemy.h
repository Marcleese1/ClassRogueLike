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
    // Constructor
    ABP_Enemy(const FObjectInitializer& ObjectInitializer);

    // Getters
    UAbilitySystemComponent* GetAbilitySystemComponent() const override;  // Enemy ASC
    UEnemyAttributeSet* GetEnemyAttributeSet() const;  // Enemy Attribute Set

    // Override for setting health
    void SetHealth(float HealthValue) override;
    void Attack();
    void OnTagChanged(FGameplayTag CallbackTag, int32 NewCount);

    void UpdateMovementSpeed();

    // Get health and max health
    float GetMaxHealth() const;
    float GetHealth() const;

    // Rotation handling
    void UpdateTargetRotation();  // Make enemies rotate correctly

    // Death handling
    virtual void Die();
    void FinishDying();

    // Add startup effects (if needed)
    void AddStartupEffects();

    // Stun mechanic handler (for future use)
    void StunTagChanged(const FGameplayTag CallbackTag, int32 NewCount);

protected:
    virtual void BeginPlay() override;
    void Tick(float DeltaTime) override;  // For rotation updates

    // Ability System Component for Enemies
    UPROPERTY(BlueprintReadOnly)
    UEnemyAbilitySystemComponent* EnemyAbilitySystemComponent;

    // Enemy Attribute Set
    UEnemyAttributeSet* EnemyAttributeSet;

    // Gameplay Effect for setting default attributes
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Abilities")
    TSubclassOf<UGameplayEffect> DefaultAttributesForEnemies;

    // UI Health Bar
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "UI")
    UWidgetComponent* UIFloatingStatusBarComponent;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "UI")
    TSubclassOf<UBP_EnemyHealthBarWidget> UIFloatingStatusBarClass;

    UPROPERTY(BlueprintReadOnly, Category = "UI")
    UBP_EnemyHealthBarWidget* UIFloatingStatusBar;

    // Add these variables to differentiate between fast and slow enemies
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enemy Type")
    bool bIsFastEnemy = false;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enemy Type")
    bool bIsSlowEnemy = false;

    // GameplayEffects for different movement speeds
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Abilities")
    TSubclassOf<UGameplayEffect> FastMovementSpeedEffect;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Abilities")
    TSubclassOf<UGameplayEffect> SlowMovementSpeedEffect;

    // Rotation variables
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Movement")
    float RotationSpeed;

    FRotator TargetRotation;

    // Delegate Handles
    FDelegateHandle HealthChangedDelegateHandle;
    FDelegateHandle MaxHealthChangedDelegateHandle;

    FGameplayTag DeadTag;

    // Functions
    void InitializeAttributes();  // Initialize attributes
    void UpdateHealthBar();  // Update health bar UI
    void HealthChanged(const FOnAttributeChangeData& Data);  // Health change callback
    void OnMaxHealthChangedDelegate(const FOnAttributeChangeData& Data);  // Max health change callback

    // Flag to track enemy death
    UPROPERTY()
    bool bIsDead = false;
};
