// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

UENUM(BlueprintType)
enum class BaseAbilityID : uint8
{
	None UMETA(DisplayName = "None"),
	Confirm UMETA(DisplayName = "Confirm"),
	Cancel UMETA(DisplayName = "Cancel"),
	Ability1 UMETA(DisplayName = "Ability1")
};

UENUM(BlueprintType)
enum class FighterAbilities : uint8
{
	Slash UMETA(DisplayName = "Slash"),
	Charge UMETA(DisplayName = "Charge"),
	Slam UMETA(DisplayName = "Slam"),
	Block UMETA(DisplayName = "Block"),
	Whirlwind UMETA(DisplayName = "Whirlwind")
};