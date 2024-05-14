#pragma once

/*#include "CharacterGameplayAbility.generated.h"*/ // Moved to the top
#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "ClassRoguelike/ClassRoguelike.h"
#include "CharacterGameplayAbility.generated.h"



/**
 *
 */
UCLASS()
class CLASSROGUELIKE_API UCharacterGameplayAbility : public UGameplayAbility
{
    GENERATED_BODY()


public:
    UCharacterGameplayAbility();

    UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Ability")
    BaseAbilityID AbilityInputID = BaseAbilityID::None; 
    
    UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Ability")
    BaseAbilityID AbilityID = BaseAbilityID::None;


    UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Ability")
    bool ActivateAbilityOnGranted = false;



    virtual void OnAvatarSet(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec) override;

};
