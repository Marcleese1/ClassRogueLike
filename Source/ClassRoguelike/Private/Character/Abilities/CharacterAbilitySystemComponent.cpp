#include "Character/Abilities/CharacterAbilitySystemComponent.h"
#include "EnhancedInputComponent.h"
#include "GameFramework/PlayerController.h"
#include "Player/MainPlayerState.h"
#include "Components/InputComponent.h"
#include "InputAction.h"
#include "InputTriggers.h"


namespace CharacterAbilitySystemComponent_Impl
{
    constexpr int32 InvalidInputID = 0;
    int32 IncrementingInputID = InvalidInputID;

    static int32 GetNextInputID()
    {
        return ++IncrementingInputID;
    }
}


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

// New function for setting input binding
void UCharacterAbilitySystemComponent::SetInputBinding(const UInputAction* InputAction, FGameplayAbilitySpecHandle AbilityHandle)
{
    using namespace CharacterAbilitySystemComponent_Impl;

    FGameplayAbilitySpec* BindingAbility = FindAbilitySpec(AbilityHandle);

    FAbilityInputBinding* AbilityInputBinding = MappedAbilities.Find(InputAction);
    if (AbilityInputBinding)
    {
        FGameplayAbilitySpec* OldBoundAbility = FindAbilitySpec(AbilityInputBinding->BoundAbilitiesStack.Top());
        if (OldBoundAbility && OldBoundAbility->InputID == AbilityInputBinding->InputID)
        {
            OldBoundAbility->InputID = InvalidInputID;
        }
    }
    else
    {
        // Cast away the constness temporarily to add the binding
        AbilityInputBinding = &MappedAbilities.Add(const_cast<UInputAction*>(InputAction));
        AbilityInputBinding->InputID = GetNextInputID();
    }

    if (BindingAbility)
    {
        BindingAbility->InputID = AbilityInputBinding->InputID;
    }

    AbilityInputBinding->BoundAbilitiesStack.Push(AbilityHandle);
    TryBindAbilityInput(const_cast<UInputAction*>(InputAction), *AbilityInputBinding);
}

// New function for clearing input binding
void UCharacterAbilitySystemComponent::ClearInputBinding(FGameplayAbilitySpecHandle AbilityHandle)
{
    using namespace CharacterAbilitySystemComponent_Impl;

    if (FGameplayAbilitySpec* FoundAbility = FindAbilitySpec(AbilityHandle))
    {
        auto MappedIterator = MappedAbilities.CreateIterator();
        while (MappedIterator)
        {
            if (MappedIterator.Value().InputID == FoundAbility->InputID)
            {
                break;
            }

            ++MappedIterator;
        }

        if (MappedIterator)
        {
            FAbilityInputBinding& AbilityInputBinding = MappedIterator.Value();

            if (AbilityInputBinding.BoundAbilitiesStack.Remove(AbilityHandle) > 0)
            {
                if (AbilityInputBinding.BoundAbilitiesStack.Num() > 0)
                {
                    FGameplayAbilitySpec* StackedAbility = FindAbilitySpec(AbilityInputBinding.BoundAbilitiesStack.Top());
                    if (StackedAbility && StackedAbility->InputID == 0)
                    {
                        StackedAbility->InputID = AbilityInputBinding.InputID;
                    }
                }
                else
                {
                    RemoveEntry(MappedIterator.Key());
                }
                FoundAbility->InputID = InvalidInputID;
            }
        }
    }
}

// New function for clearing ability bindings
void UCharacterAbilitySystemComponent::ClearAbilityBindings(UInputAction* InputAction)
{
    RemoveEntry(InputAction);
}

// New function for handling ability input press
void UCharacterAbilitySystemComponent::OnAbilityInputPressed(UInputAction* InputAction)
{
    using namespace CharacterAbilitySystemComponent_Impl;

    FAbilityInputBinding* FoundBinding = MappedAbilities.Find(InputAction);
    if (FoundBinding && ensure(FoundBinding->InputID != InvalidInputID))
    {
        AbilityLocalInputPressed(FoundBinding->InputID);
    }
}

// New function for handling ability input release
void UCharacterAbilitySystemComponent::OnAbilityInputReleased(UInputAction* InputAction)
{
    using namespace CharacterAbilitySystemComponent_Impl;

    FAbilityInputBinding* FoundBinding = MappedAbilities.Find(InputAction);
    if (FoundBinding && ensure(FoundBinding->InputID != InvalidInputID))
    {
        AbilityLocalInputReleased(FoundBinding->InputID);
    }
}

// New function for removing an entry
void UCharacterAbilitySystemComponent::RemoveEntry(UInputAction* InputAction)
{
    if (FAbilityInputBinding* Bindings = MappedAbilities.Find(InputAction))
    {
        if (InputComponent)
        {
            InputComponent->RemoveBindingByHandle(Bindings->OnPressedHandle);
            InputComponent->RemoveBindingByHandle(Bindings->OnReleasedHandle);
        }

        for (FGameplayAbilitySpecHandle AbilityHandle : Bindings->BoundAbilitiesStack)
        {
            using namespace CharacterAbilitySystemComponent_Impl;

            FGameplayAbilitySpec* AbilitySpec = FindAbilitySpec(AbilityHandle);
            if (AbilitySpec && AbilitySpec->InputID == Bindings->InputID)
            {
                AbilitySpec->InputID = InvalidInputID;
            }
        }

        MappedAbilities.Remove(InputAction);
    }
}

// New function for finding an ability spec
FGameplayAbilitySpec* UCharacterAbilitySystemComponent::FindAbilitySpec(FGameplayAbilitySpecHandle Handle)
{
    FGameplayAbilitySpec* FoundAbility = nullptr;
    FoundAbility = FindAbilitySpecFromHandle(Handle);
    return FoundAbility;
}

// New function for binding ability input
void UCharacterAbilitySystemComponent::TryBindAbilityInput(UInputAction* InputAction, FAbilityInputBinding& AbilityInputBinding)
{
    if (InputComponent)
    {
        if (AbilityInputBinding.OnPressedHandle == 0)
        {
            AbilityInputBinding.OnPressedHandle = InputComponent->BindAction(InputAction, ETriggerEvent::Started, this, &UCharacterAbilitySystemComponent::OnAbilityInputPressed, InputAction).GetHandle();
        }

        if (AbilityInputBinding.OnReleasedHandle == 0)
        {
            AbilityInputBinding.OnReleasedHandle = InputComponent->BindAction(InputAction, ETriggerEvent::Completed, this, &UCharacterAbilitySystemComponent::OnAbilityInputReleased, InputAction).GetHandle();
        }
    }
}

void UCharacterAbilitySystemComponent::ProcessInputAction(const UInputAction* InputAction, bool bPressed)
{
    FGameplayAbilitySpecHandle* AbilityHandlePtr = InputToAbilityMap.Find(InputAction);
    if (AbilityHandlePtr)
    {
        FGameplayAbilitySpecHandle AbilityHandle = *AbilityHandlePtr;
        if (bPressed)
        {
            OnAbilityInputPressed(const_cast<UInputAction*>(InputAction));  // Removed const_cast
        }
        else
        {
            OnAbilityInputReleased(const_cast<UInputAction*>(InputAction)); // Removed const_cast
        }
    }
}