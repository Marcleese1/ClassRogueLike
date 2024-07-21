#pragma once

#include "CoreMinimal.h"
#include "CharacterBase.h"
#include "GameFramework/Character.h"
#include "AbilitySystemInterface.h"
#include "GameplayTagContainer.h"
#include "CharacterBase.h"
#include "GameplayAbilitySpec.h"
#include "Enemy/AttributeSet/EnemyAttributeSet.h"
#include "Components/WidgetComponent.h"
#include "BP_Enemy.generated.h"

UCLASS(config = Game)
class CLASSROGUELIKE_API ABP_Enemy : public ACharacterBase
{
    GENERATED_BODY()

public:
    ABP_Enemy(const class FObjectInitializer& ObjectInitializer);

    virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;

   /* UPROPERTY(BlueprintAssignable, Category = "ClassRoguelike|Enemy")
    FEnemyDiedDelegate OnEnemyDied;*/

    virtual void RemoveCharacterAbilities() override;
    virtual void Die() override;
    virtual void FinishDying() override;


protected:
    virtual void BeginPlay() override;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Abilities", meta = (AllowPrivateAccess = "true"))
    UEnemyAttributeSet* EnemyAttributeSet;

    UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "ClassRoguelike|UI")
    TSubclassOf<class UBP_EnemyHealthBarWidget> UIFloatingStatusBarClass;

    UPROPERTY()
    class UBP_EnemyHealthBarWidget* UIFloatingStatusBar;

    UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "ClassRoguelike|UI")
    class UWidgetComponent* UIFloatingStatusBarComponent;

    FDelegateHandle HealthChangedDelegateHandle;

    virtual void HealthChanged(const FOnAttributeChangeData& Data);
    virtual void UpdateHealthBar();

    FGameplayTag DeadTag;
    FGameplayTag EffectRemoveOnDeathTag;
};