// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy/AttributeSet/EnemyAttributeSet.h"

#include "Net/UnrealNetwork.h"

UEnemyAttributeSet::UEnemyAttributeSet()
{
    Health.SetCurrentValue(100.0f);
    Health.SetBaseValue(100.0f);

    MovementSpeed.SetCurrentValue(600.0f); // Default movement speed
    MovementSpeed.SetBaseValue(600.0f);
}

void UEnemyAttributeSet::OnRep_Health(const FGameplayAttributeData& OldHealth)
{
    GAMEPLAYATTRIBUTE_REPNOTIFY(UEnemyAttributeSet, Health, OldHealth);
}

void UEnemyAttributeSet::OnRep_Damage(const FGameplayAttributeData& OldDamage)
{
    GAMEPLAYATTRIBUTE_REPNOTIFY(UEnemyAttributeSet, Damage, OldDamage);
}

void UEnemyAttributeSet::OnRep_MovementSpeed(const FGameplayAttributeData& OldMovementSpeed)
{
    GAMEPLAYATTRIBUTE_REPNOTIFY(UEnemyAttributeSet, MovementSpeed, OldMovementSpeed);
}

void UEnemyAttributeSet::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    DOREPLIFETIME_CONDITION_NOTIFY(UEnemyAttributeSet, Health, COND_None, REPNOTIFY_Always);
    DOREPLIFETIME_CONDITION_NOTIFY(UEnemyAttributeSet, Damage, COND_None, REPNOTIFY_Always);
    DOREPLIFETIME_CONDITION_NOTIFY(UEnemyAttributeSet, MovementSpeed, COND_None, REPNOTIFY_Always);
}