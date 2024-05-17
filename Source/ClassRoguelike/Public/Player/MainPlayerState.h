// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "AbilitySystemInterface.h"
#include "Character/Abilities/AttributeSets/CharacterAttributeSetBase.h"
#include "GameplayEffectTypes.h"
#include "MainPlayerState.generated.h"

/**
 * 
 */
UCLASS()
class CLASSROGUELIKE_API AMainPlayerState : public APlayerState, public IAbilitySystemInterface
{
	GENERATED_BODY()

public:
	AMainPlayerState();

	class UAbilitySystemComponent* GetAbilitySystemComponent() const override;

	class UCharacterAttributeSetBase* GetAttributeSetBase();
	

	UFUNCTION(BlueprintCallable, Category = "ClassRogueLike|MainPlayerState")
	bool IsAlive() const;


	UFUNCTION(BlueprintCallable, Category = "ClassRogueLike|MainPlayerState|UI")
	void ShowAbilityConfirmCancelText(bool ShowText);
	

	UFUNCTION(BlueprintCallable, Category = "ClassRogueLike|MainPlayerState|Attributes")
	float GetHealth() const;
	UFUNCTION(BlueprintCallable, Category = "ClassRogueLike|MainPlayerState|Attributes")
	float GetMaxHealth() const;

	UFUNCTION(BlueprintCallable, Category = "ClassRogueLike|MainPlayerState|Attributes")
	float GetMana() const;
	UFUNCTION(BlueprintCallable, Category = "ClassRogueLike|MainPlayerState|Attributes")
	float GetMaxMana() const;

	UFUNCTION(BlueprintCallable, Category = "ClassRogueLike|MainPlayerState|Attributes")
	int32 GetCharacterLevel() const;


protected:

	UPROPERTY()
	class UCharacterAbilitySystemComponent* AbilitySystemComponent;

	UPROPERTY()
	class UCharacterAttributeSetBase* AttributeSetBase;

	FGameplayTag DeadTag;


	FDelegateHandle HealthChangedDelegateHandle;
	FDelegateHandle ManaChangedDelegateHandle;
	FDelegateHandle MaxHealthChangedDelegateHandle;
	FDelegateHandle MaxManaChangedDelegateHandle;
	FDelegateHandle CharacterLevelChangedDelegateHandle;

	virtual void BeginPlay() override;

	virtual void HealthChanged(const FOnAttributeChangeData& Data);
	virtual void MaxHealthChanged(const FOnAttributeChangeData& Data);
	virtual void ManaChanged(const FOnAttributeChangeData& Data);
	virtual void MaxManaChanged(const FOnAttributeChangeData& Data);
	virtual void CharacterLevelChanged(const FOnAttributeChangeData& Data);
	 

	virtual void StunTagChanged(const FGameplayTag CallbackTag, int32 NewCount);
};
