// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy/AttributeSet/EnemyAttributeSet.h"
#include "Net/UnrealNetwork.h"
#include "Components/CapsuleComponent.h"
#include "Components/WidgetComponent.h"
#include "Kismet/GameplayStatics.h"
#include "AbilitySystemComponent.h"
//#include "UI/GDFloatingStatusBarWidget.h"

UEnemyAttributeSet::UEnemyAttributeSet()
{
    Health.SetBaseValue(100.0f);
    Health.SetCurrentValue(100.0f);

    MaxHealth.SetBaseValue(100.0f);
    MaxHealth.SetCurrentValue(100.0f);

    Damage.SetBaseValue(10.0f);
    Damage.SetCurrentValue(10.0f);

    MovementSpeed.SetBaseValue(600.0f);
    MovementSpeed.SetCurrentValue(600.0f);
}

void UEnemyAttributeSet::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    DOREPLIFETIME(UEnemyAttributeSet, Health);
    DOREPLIFETIME(UEnemyAttributeSet, MaxHealth);
    DOREPLIFETIME(UEnemyAttributeSet, Damage);
    DOREPLIFETIME(UEnemyAttributeSet, MovementSpeed);
}

void UEnemyAttributeSet::OnRep_Health(const FGameplayAttributeData& OldHealth)
{
    GAMEPLAYATTRIBUTE_REPNOTIFY(UEnemyAttributeSet, Health, OldHealth);
}

void UEnemyAttributeSet::OnRep_MaxHealth(const FGameplayAttributeData& OldMaxHealth)
{
    GAMEPLAYATTRIBUTE_REPNOTIFY(UEnemyAttributeSet, MaxHealth, OldMaxHealth);
}

void UEnemyAttributeSet::OnRep_Damage(const FGameplayAttributeData& OldDamage)
{
    GAMEPLAYATTRIBUTE_REPNOTIFY(UEnemyAttributeSet, Damage, OldDamage);
}

void UEnemyAttributeSet::OnRep_MovementSpeed(const FGameplayAttributeData& OldMovementSpeed)
{
    GAMEPLAYATTRIBUTE_REPNOTIFY(UEnemyAttributeSet, MovementSpeed, OldMovementSpeed);
}