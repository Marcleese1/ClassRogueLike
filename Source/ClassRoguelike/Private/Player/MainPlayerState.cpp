// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/MainPlayerState.h"
#include "Character/Abilities/AttributeSets/CharacterAttributeSetBase.h"
#include "Character/Abilities/CharacterAbilitySystemComponent.h"


AMainPlayerState::AMainPlayerState()
{
	AbilitySystemComponent = CreateDefaultSubobject<UCharacterAbilitySystemComponent>(TEXT("AbilitySystemComponent"));
	AbilitySystemComponent->SetIsReplicated(true);
	AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Mixed);

	AttributeSetBase = CreateDefaultSubobject <UCharacterAttributeSetBase>(TEXT("AttributeSetBase"));

	NetUpdateFrequency = 100.0f;

	DeadTag = FGameplayTag::RequestGameplayTag(FName("State.Dead"));
}

UAbilitySystemComponent* AMainPlayerState::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}

UCharacterAttributeSetBase* AMainPlayerState::GetAttributeSetBase()
{
	return AttributeSetBase;
}

bool AMainPlayerState::IsAlive() const
{
	return GetHealth() > 0;
}

void AMainPlayerState::ShowAbilityConfirmCancelText(bool ShowText)
{
	//TODO -- Implement HUD Later
}

float AMainPlayerState::GetHealth() const
{
	return AttributeSetBase->GetHealth();
}

float AMainPlayerState::GetMaxHealth() const
{
	return AttributeSetBase->GetMaxHealth();
}

float AMainPlayerState::GetMana() const
{
	return AttributeSetBase->GetMana();
}

float AMainPlayerState::GetMaxMana() const
{
	return AttributeSetBase->GetMaxMana();
}

float AMainPlayerState::GetStamina() const
{
	return AttributeSetBase->GetStamina();
}

float AMainPlayerState::GetMaxStamina() const
{
	return AttributeSetBase->GetMaxStamina();
}

int32 AMainPlayerState::GetCharacterLevel() const
{
	return AttributeSetBase->GetLevel();
}

void AMainPlayerState::BeginPlay()
{
	Super::BeginPlay();

	if (AbilitySystemComponent)
	{
		HealthChangedDelegateHandle = AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(AttributeSetBase->GetHealthAttribute()).AddUObject(this, &AMainPlayerState::HealthChanged);
		MaxHealthChangedDelegateHandle = AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(AttributeSetBase->GetMaxHealthAttribute()).AddUObject(this, &AMainPlayerState::MaxHealthChanged);
		ManaChangedDelegateHandle = AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(AttributeSetBase->GetManaAttribute()).AddUObject(this, &AMainPlayerState::ManaChanged);
		MaxManaChangedDelegateHandle = AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(AttributeSetBase->GetMaxManaAttribute()).AddUObject(this, &AMainPlayerState::MaxManaChanged);
		CharacterLevelChangedDelegateHandle = AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(AttributeSetBase->GetLevelAttribute()).AddUObject(this, &AMainPlayerState::CharacterLevelChanged);

		AbilitySystemComponent->RegisterGameplayTagEvent(FGameplayTag::RequestGameplayTag(FName("State.Debuff.Stun")), EGameplayTagEventType::NewOrRemoved).AddUObject(this, &AMainPlayerState::StunTagChanged);
	}
}

void AMainPlayerState::HealthChanged(const FOnAttributeChangeData& Data)
{
	UE_LOG(LogTemp, Warning, TEXT("Health Changed"));
}

void AMainPlayerState::MaxHealthChanged(const FOnAttributeChangeData& Data)
{
	UE_LOG(LogTemp, Warning, TEXT("Max Health Changed"));
}

void AMainPlayerState::ManaChanged(const FOnAttributeChangeData& Data)
{
	UE_LOG(LogTemp, Warning, TEXT("Mana Changed"));
}

void AMainPlayerState::MaxManaChanged(const FOnAttributeChangeData& Data)
{
	UE_LOG(LogTemp, Warning, TEXT("Max Mana Changed"));
}
void AMainPlayerState::StaminaChanged(const FOnAttributeChangeData& Data)
{
	UE_LOG(LogTemp, Warning, TEXT("Stamina Changed"));
}

void AMainPlayerState::MaxStaminaChanged(const FOnAttributeChangeData& Data)
{
	UE_LOG(LogTemp, Warning, TEXT("Max Stamina Changed"));
}

void AMainPlayerState::CharacterLevelChanged(const FOnAttributeChangeData& Data)
{
	UE_LOG(LogTemp, Warning, TEXT("Level Changed"));
}

void AMainPlayerState::StunTagChanged(const FGameplayTag CallbackTag, int32 NewCount)
{

	if (NewCount > 0)
	{
		FGameplayTagContainer AbilityTagsToCancel;
		AbilityTagsToCancel.AddTag(FGameplayTag::RequestGameplayTag(FName("Ability")));

		FGameplayTagContainer AbilityTagsToIgnore;
		AbilityTagsToCancel.AddTag(FGameplayTag::RequestGameplayTag(FName("Ability.NotCanceledByStun")));

		AbilitySystemComponent->CancelAbilities(&AbilityTagsToCancel, &AbilityTagsToIgnore);
	}

	UE_LOG(LogTemp, Warning, TEXT("Stun Changed"));
}