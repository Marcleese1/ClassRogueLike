// Fill out your copyright notice in the Description page of Project Settings.


#include "CharacterBase.h"
#include "Components/CapsuleComponent.h"
#include "Character/Abilities/AttributeSets/CharacterAttributeSetBase.h"
#include "Character/Abilities/CharacterAbilitySystemComponent.h"
#include "Character/Abilities/CharacterGameplayAbility.h"
#include "GameFramework/CharacterMovementComponent.h"

// Sets default values
ACharacterBase::ACharacterBase(const class FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer.SetDefaultSubobjectClass<UCharacterMovementComponent>(ACharacter::CharacterMovementComponentName))
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	GetCapsuleComponent()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility, ECollisionResponse::ECR_Overlap);

	bAlwaysRelevant = true;

	DeadTag = FGameplayTag::RequestGameplayTag(FName("State.Dead"));
	EffectRemoveOnDeathTag = FGameplayTag::RequestGameplayTag(FName("State.RemoveOnDeath"));
}

UAbilitySystemComponent* ACharacterBase::GetAbilitySystemComponent() const {
	return AbilitySystemComponent.Get();
}

// Called when the game starts or when spawned
void ACharacterBase::BeginPlay()
{
	Super::BeginPlay();
	
}

bool ACharacterBase::IsAlive() const
{
	return GetHealth()>0.0f;
}

int32 ACharacterBase::GetAbilityLevel(BaseAbilityID AbilityID) const
{
	return 1;
}

void ACharacterBase::RemoveCharacterAbilities()
{
	if (GetLocalRole() != ROLE_Authority || !AbilitySystemComponent.IsValid() || !AbilitySystemComponent->CharacterAbilitiesGiven) {
		return;
	}

	TArray<FGameplayAbilitySpecHandle> AbilitiesToRemove;
	for (const FGameplayAbilitySpec& Spec : AbilitySystemComponent->GetActivatableAbilities()) {
		if ((Spec.SourceObject == this) && CharacterAbilities.Contains(Spec.Ability->GetClass())) {
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
	if (AttributeSetBase.IsValid()) {
		return AttributeSetBase->GetLevel();
	}

	return 0.0f;
}

float ACharacterBase::GetHealth() const
{
	if (AttributeSetBase.IsValid()) {
		return AttributeSetBase->GetHealth();
		

	}

	return 0.0f;
}

float ACharacterBase::GetMana() const
{
	if (AttributeSetBase.IsValid()) {
		return AttributeSetBase->GetMana();
	}

	return 0.0f;
}

float ACharacterBase::GetMaxHealth() const
{
	if (AttributeSetBase.IsValid()) {
		return AttributeSetBase->GetMaxHealth();
	}

	return 0.0f;
}

float ACharacterBase::GetMaxMana() const
{
	if (AttributeSetBase.IsValid()) {
		return AttributeSetBase->GetMaxMana();
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

	if (AbilitySystemComponent.IsValid()) {
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
	if (GetLocalRole() != ROLE_Authority || !AbilitySystemComponent.IsValid() || !AbilitySystemComponent->CharacterAbilitiesGiven) {
		return;
	}

	for (TSubclassOf<UCharacterGameplayAbility>& StartupAbility : CharacterAbilities) {
		AbilitySystemComponent->GiveAbility(FGameplayAbilitySpec(StartupAbility, GetAbilityLevel(StartupAbility.GetDefaultObject()->AbilityID), static_cast<int32>(StartupAbility.GetDefaultObject()->AbilityID), this));
	}

	AbilitySystemComponent->CharacterAbilitiesGiven = true;
}


void ACharacterBase::InitializeAttributes()
{
	if (!AbilitySystemComponent.IsValid())
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
		FActiveGameplayEffectHandle ActiveGEHandle = AbilitySystemComponent->ApplyGameplayEffectSpecToTarget(*NewHandle.Data.Get(), AbilitySystemComponent.Get());
	}

}

void ACharacterBase::AddStartupEffects()
{
	if (GetLocalRole() != ROLE_Authority || !AbilitySystemComponent.IsValid() || !AbilitySystemComponent->StartupEffectsApplied) {
		return;
	}
	FGameplayEffectContextHandle EffectContext = AbilitySystemComponent->MakeEffectContext();
	EffectContext.AddSourceObject(this);

	for (TSubclassOf<UGameplayEffect> GameplayEffect : StartupEffects)
	{
		FGameplayEffectSpecHandle NewHandle = AbilitySystemComponent->MakeOutgoingSpec(GameplayEffect, GetCharacterLevel(), EffectContext);
		if (NewHandle.IsValid())
		{
			FActiveGameplayEffectHandle ActiveGEHandle = AbilitySystemComponent->ApplyGameplayEffectSpecToTarget(*NewHandle.Data.Get(), AbilitySystemComponent.Get());
		}
	}

	AbilitySystemComponent->StartupEffectsApplied = true;
}


void ACharacterBase::SetHealth(float Health)
{
	if (AttributeSetBase.IsValid()) {
		AttributeSetBase->SetHealth(Health);
	}
}

void ACharacterBase::SetMana(float Mana)
{
	if (AttributeSetBase.IsValid()) {
		AttributeSetBase->SetHealth(Mana);
	}
}

void ACharacterBase::FinishDying()
{
	Destroy();
}

