#pragma once
//
#include "CoreMinimal.h"
#include "CharacterBase.h"
#include "GameplayEffectTypes.h"
#include "BP_Enemy.generated.h"
//
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

protected:
    virtual void BeginPlay() override;

   /* UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    UCharacterAbilitySystemComponent* AbilitySystemComponent;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    UCharacterAttributeSetBase* AttributeSetBase;*/

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "UI")
    UWidgetComponent* UIFloatingStatusBarComponent;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "UI")
    TSubclassOf<UBP_EnemyHealthBarWidget> UIFloatingStatusBarClass;

    UPROPERTY(BlueprintReadOnly, Category = "UI")
    UBP_EnemyHealthBarWidget* UIFloatingStatusBar;

    virtual void OnHealthChanged(const FOnAttributeChangeData& Data);

    FDelegateHandle HealthChangedDelegateHandle;

    virtual void Die() override;

    virtual void FinishDying() override;
    void OnSeePawn(APawn* Pawn);
};
