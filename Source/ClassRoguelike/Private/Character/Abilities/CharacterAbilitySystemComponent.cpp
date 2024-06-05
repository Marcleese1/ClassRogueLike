#include "Character/Abilities/CharacterAbilitySystemComponent.h"
#include "EnhancedInputComponent.h"
#include "GameFramework/PlayerController.h"
#include "Player/MainPlayerState.h"
#include "Components/InputComponent.h"
#include "InputAction.h"
#include "InputTriggers.h"

void UCharacterAbilitySystemComponent::ReceiveDamage(UCharacterAbilitySystemComponent* SourceASC, float UnmitigatedDamage, float MitigatedDamage)
{
    ReceivedDamage.Broadcast(SourceASC, UnmitigatedDamage, MitigatedDamage);
}

void UCharacterAbilitySystemComponent::OnGiveAbility(FGameplayAbilitySpec& AbilitySpec)
{
    Super::OnGiveAbility(AbilitySpec);
    if (!bInputComponentInitialized)
    {
        InitializeInputComponent();
    }
}

void UCharacterAbilitySystemComponent::InitializeInputComponent()
{
    if (AMainPlayerState* Owner = Cast<AMainPlayerState>(GetOwner()))
    {
        if (APawn* OwnerPawn = Owner->GetPawn())
        {
            if (APlayerController* PC = Cast<APlayerController>(OwnerPawn->GetController()))
            {
                if (PC->InputComponent)
                {
                    InputComponent = Cast<UEnhancedInputComponent>(PC->InputComponent);
                    if (InputComponent)
                    {
                        bInputComponentInitialized = true;
                    }
                    else
                    {
                        UE_LOG(LogTemp, Error, TEXT("InputComponent is not UEnhancedInputComponent"));
                    }
                }
            }
        }
    }
}

void UCharacterAbilitySystemComponent::SetInputBinding(const UInputAction* InputAction, FGameplayAbilitySpecHandle AbilityHandle)
{
    if (!InputComponent)
    {
        UE_LOG(LogTemp, Error, TEXT("InputComponent is not initialized"));
        return;
    }

    if (AbilityHandle.IsValid())
    {
        InputToAbilityMap.Add(InputAction, AbilityHandle);
    }
}

void UCharacterAbilitySystemComponent::ProcessInputAction(const UInputAction* InputAction, bool bPressed)
{
    if (FGameplayAbilitySpecHandle* AbilityHandlePtr = InputToAbilityMap.Find(InputAction))
    {
        if (bPressed)
        {
            OnAbilityInputPressed(*AbilityHandlePtr);
        }
        else
        {
            OnAbilityInputReleased(*AbilityHandlePtr);
        }
    }
}

void UCharacterAbilitySystemComponent::OnAbilityInputPressed(FGameplayAbilitySpecHandle AbilityHandle)
{
    TryActivateAbility(AbilityHandle);
}

void UCharacterAbilitySystemComponent::OnAbilityInputReleased(FGameplayAbilitySpecHandle AbilityHandle)
{
    // Handle ability release logic here if needed
}
