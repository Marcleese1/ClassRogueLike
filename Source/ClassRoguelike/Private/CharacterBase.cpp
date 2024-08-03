#include "CharacterBase.h"
#include "Components/CapsuleComponent.h"
#include "Character/Abilities/AttributeSets/CharacterAttributeSetBase.h"
#include "Character/Abilities/CharacterAbilitySystemComponent.h"
#include "Character/Abilities/CharacterGameplayAbility.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "EnhancedPlayerInput.h"

// Sets default values
// Sets default values
ACharacterBase::ACharacterBase(const FObjectInitializer& ObjectInitializer)
    : Super(ObjectInitializer) // Properly call the base class constructor
{
    // Initialize the ability system component
    AbilitySystemComponent = CreateDefaultSubobject<UCharacterAbilitySystemComponent>(TEXT("AbilitySystemComponent"));
    AttributeSetBase = CreateDefaultSubobject<UCharacterAttributeSetBase>(TEXT("AttributeSetBase"));

    // Set replication properties
    AbilitySystemComponent->SetIsReplicated(true);
    AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Minimal);

    // Initialize default values and properties
    DeadTag = FGameplayTag::RequestGameplayTag(FName("State.Dead"));
    /*EffectRemoveOnDeathTag = FGameplayTag::RequestGameplayTag(FName("Effect.RemoveOnDeath"));*/
}

UAbilitySystemComponent* ACharacterBase::GetAbilitySystemComponent() const {
    return AbilitySystemComponent;
}

// Called when the game starts or when spawned
void ACharacterBase::BeginPlay()
{
    Super::BeginPlay();
}

bool ACharacterBase::IsAlive() const
{
    return GetHealth() > 0.0f;
}

int32 ACharacterBase::GetAbilityLevel(BaseAbilityID AbilityID) const
{
    return 1;
}

void ACharacterBase::RemoveCharacterAbilities()
{
    if (GetLocalRole() != ROLE_Authority || !AbilitySystemComponent || !AbilitySystemComponent->CharacterAbilitiesGiven) {
        return;
    }

    TArray<FGameplayAbilitySpecHandle> AbilitiesToRemove;
    for (const FGameplayAbilitySpec& Spec : AbilitySystemComponent->GetActivatableAbilities()) {
        // Loop through the map to find if the ability class is in the map
        bool bContainsAbility = false;
        for (const auto& Entry : CharacterAbilities)
        {
            if (Entry.Value == Spec.Ability->GetClass())
            {
                bContainsAbility = true;
                break;
            }
        }
        if (bContainsAbility) {
            AbilitiesToRemove.Add(Spec.Handle);
        }
    }

    for (int32 i = 0; i < AbilitiesToRemove.Num(); i++) {
        AbilitySystemComponent->ClearAbility(AbilitiesToRemove[i]);
    }

    AbilitySystemComponent->CharacterAbilitiesGiven = false;
}

float ACharacterBase::GetCharacterLevel() const
{
    if (AttributeSetBase) {
        return AttributeSetBase->GetLevel();
    }

    return 0.0f;
}

float ACharacterBase::GetHealth() const
{
    if (AttributeSetBase) {
        return AttributeSetBase->GetHealth();
    }

    return 0.0f;
}

float ACharacterBase::GetMana() const
{
    if (AttributeSetBase) {
        return AttributeSetBase->GetMana();
    }

    return 0.0f;
}

float ACharacterBase::GetMaxHealth() const
{
    if (AttributeSetBase) {
        return AttributeSetBase->GetMaxHealth();
    }

    return 0.0f;
}

float ACharacterBase::GetMaxMana() const
{
    if (AttributeSetBase) {
        return AttributeSetBase->GetMaxMana();
    }

    return 0.0f;

}

float ACharacterBase::GetStamina() const
{
    if (AttributeSetBase) {
        return AttributeSetBase->GetMana();
    }

    return 0.0f;
}

float ACharacterBase::GetMaxStamina() const
{
    if (AttributeSetBase) {
        return AttributeSetBase->GetMaxStamina();
    }

    return 0.0f;
}

void ACharacterBase::Die()
{
    RemoveCharacterAbilities();
    GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    GetCharacterMovement()->GravityScale = 0;
    GetCharacterMovement()->Velocity = FVector(0);

    OnCharacterDied.Broadcast(this);

    if (AbilitySystemComponent) {
        AbilitySystemComponent->CancelAbilities();

        FGameplayTagContainer EffectsTagsToRemove;
        EffectsTagsToRemove.AddTag(EffectRemoveOnDeathTag);
        int32 NumEffectsRemoved = AbilitySystemComponent->RemoveActiveEffectsWithTags(EffectsTagsToRemove);
        AbilitySystemComponent->AddLooseGameplayTag(DeadTag);
    }

    if (DeathMontage) {
        PlayAnimMontage(DeathMontage);
    }
    else {
        FinishDying();
    }
}

void ACharacterBase::AddCharacterAbilities()
{
    if (GetLocalRole() != ROLE_Authority || !AbilitySystemComponent || AbilitySystemComponent->CharacterAbilitiesGiven) {
        return;
    }

    for (const auto& Entry : CharacterAbilities)
    {
        // Get the InputAction and GameplayAbility from the TMap
        UInputAction* InputAction = Entry.Key;
        TSubclassOf<UCharacterGameplayAbility> StartupAbility = Entry.Value;
        UE_LOG(LogTemp, Warning, TEXT("Giving Abilities to AbilitySystemComponent with default values..."));
        FGameplayAbilitySpecHandle AbilityHandle = AbilitySystemComponent->GiveAbility(
            FGameplayAbilitySpec(StartupAbility, 1, 0, this));
        UE_LOG(LogTemp, Warning, TEXT("Ability Name: %s"), *StartupAbility.GetDefaultObject()->GetName());

        // Add the InputAction and AbilityHandle to the map
        UE_LOG(LogTemp, Warning, TEXT("Adding InputAction %s and AbilityHandle %s to Map"), *InputAction->GetName(), *AbilityHandle.ToString());
        InputToAbilityMap.Add(InputAction, AbilityHandle);
    }

    AbilitySystemComponent->CharacterAbilitiesGiven = true;
}

void ACharacterBase::InitializeAttributes()
{
    if (!AbilitySystemComponent)
    {
        return;
    }
    if (!DefaultAttributes)
    {
        UE_LOG(LogTemp, Error, TEXT("%s() Missing DefaultAttributes for %s. Please Fill in the Characters Blueprint."), *FString(__FUNCTION__), *GetName());
    }

    FGameplayEffectContextHandle EffectContext = AbilitySystemComponent->MakeEffectContext();
    EffectContext.AddSourceObject(this);

    FGameplayEffectSpecHandle NewHandle = AbilitySystemComponent->MakeOutgoingSpec(DefaultAttributes, GetCharacterLevel(), EffectContext);
    if (NewHandle.IsValid())
    {
        FActiveGameplayEffectHandle ActiveGEHandle = AbilitySystemComponent->ApplyGameplayEffectSpecToTarget(*NewHandle.Data.Get(), AbilitySystemComponent);

    }
}

void ACharacterBase::AddStartupEffects()
{
    if (GetLocalRole() != ROLE_Authority || !AbilitySystemComponent || AbilitySystemComponent->StartupEffectsApplied) {
        return;
    }
    FGameplayEffectContextHandle EffectContext = AbilitySystemComponent->MakeEffectContext();
    EffectContext.AddSourceObject(this);

    for (TSubclassOf<UGameplayEffect> GameplayEffect : StartupEffects)
    {
        FGameplayEffectSpecHandle NewHandle = AbilitySystemComponent->MakeOutgoingSpec(GameplayEffect, GetCharacterLevel(), EffectContext);
        if (NewHandle.IsValid())
        {
            FActiveGameplayEffectHandle ActiveGEHandle = AbilitySystemComponent->ApplyGameplayEffectSpecToTarget(*NewHandle.Data.Get(), AbilitySystemComponent);
        }
    }

    AbilitySystemComponent->StartupEffectsApplied = true;
}

void ACharacterBase::SetHealth(float Health)
{
    if (AttributeSetBase) {
        AttributeSetBase->SetHealth(Health);
    }
}

void ACharacterBase::SetMana(float Mana)
{
    if (AttributeSetBase) {
        AttributeSetBase->SetMana(Mana);
    }
}

void ACharacterBase::SetStamina(float Stamina)
{
    if (AttributeSetBase) {
        AttributeSetBase->SetStamina(Stamina);
    }
}

//void ACharacterBase::OnHealthChanged(const FOnAttributeChangeData& Data)
//{
//    float NewHealth = Data.NewValue;
//
//    UE_LOG(LogTemp, Log, TEXT("Health Changed: %f"), NewHealth);
//
//    // Update health bar, can be overridden in derived classes
//    UpdateHealthBar();
//
//    // Handle death
//    if (!IsAlive() && !AbilitySystemComponent->HasMatchingGameplayTag(DeadTag))
//    {
//        Die();
//    }
//}

//void ACharacterBase::UpdateHealthBar()
//{
//    // Placeholder for update logic, should be overridden in subclasses if needed
//    // This is where ABP_Enemy can override to update specific UI elements
//}

void ACharacterBase::FinishDying()
{
    Destroy();
}