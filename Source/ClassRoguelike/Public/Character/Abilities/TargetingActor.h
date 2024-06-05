// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbilityTargetActor.h"
#include "GameFramework/Actor.h"
#include "TargetingActor.generated.h"

UCLASS()
class CLASSROGUELIKE_API ATargetingActor : public AGameplayAbilityTargetActor
{
	GENERATED_BODY()
	
public:
    ATargetingActor();

    virtual void StartTargeting(UGameplayAbility* InAbility) override;
    virtual void Tick(float DeltaSeconds) override;

protected:
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Targeting")
    UDecalComponent* TargetDecal;

private:
    FVector StartMousePosition;
};

