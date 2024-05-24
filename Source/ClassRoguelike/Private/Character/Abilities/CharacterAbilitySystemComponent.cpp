// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/Abilities/CharacterAbilitySystemComponent.h"
#include "EnhancedInputComponent.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/PlayerState.h"


void UCharacterAbilitySystemComponent::ReceiveDamage(UCharacterAbilitySystemComponent* SourceASC, float UnmitigatedDamage, float MitigatedDamage)
{
	ReceivedDamage.Broadcast(SourceASC, UnmitigatedDamage, MitigatedDamage);
}

void UCharacterAbilitySystemComponent::OnGiveAbility(FGameplayAbilitySpec& AbilitySpec) // UPDATED
{
    Super::OnGiveAbility(AbilitySpec);
    if (!bInputComponentInitialized)
    {
        InitializeInputComponent();
    }
}

void UCharacterAbilitySystemComponent::InitializeInputComponent() // UPDATED
{
    APlayerController* PC = Cast<APlayerController>(GetOwner());
    if (PC && PC->InputComponent)
    {
        /* InputComponent = CastChecked<UEnhancedInputComponent>(PC->InputComponent);*/
        bInputComponentInitialized = true;
    }
}