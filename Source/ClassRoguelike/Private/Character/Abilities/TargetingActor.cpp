// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/Abilities/TargetingActor.h"
#include "Components/DecalComponent.h"
#include "GameFramework/PlayerController.h"
#include "Kismet/GameplayStatics.h"
#include <Character/Abilities/CharacterGameplayAbility.h>

// Sets default values
// Sets default values
ATargetingActor::ATargetingActor()
{
    PrimaryActorTick.bCanEverTick = true;

    TargetDecal = CreateDefaultSubobject<UDecalComponent>(TEXT("TargetDecal"));
    TargetDecal->SetupAttachment(RootComponent);
}

void ATargetingActor::StartTargeting(UGameplayAbility* InAbility)
{
    Super::StartTargeting(InAbility);

    OwningAbility = InAbility;

    APlayerController* PC = Cast<APlayerController>(OwningAbility->GetCurrentActorInfo()->PlayerController.Get());
    if (PC)
    {
        PC->GetMousePosition(StartMousePosition.X, StartMousePosition.Y);
    }
}

void ATargetingActor::Tick(float DeltaSeconds)
{
    Super::Tick(DeltaSeconds);

    APlayerController* PC = Cast<APlayerController>(OwningAbility->GetCurrentActorInfo()->PlayerController.Get());
    if (PC)
    {
        FVector WorldLocation, WorldDirection;
        if (PC->DeprojectMousePositionToWorld(WorldLocation, WorldDirection))
        {
            FVector End = WorldLocation + (WorldDirection * 10000.f);
            FHitResult HitResult;
            GetWorld()->LineTraceSingleByChannel(HitResult, WorldLocation, End, ECC_Visibility);
            if (HitResult.bBlockingHit)
            {
                SetActorLocation(HitResult.Location);
                TargetDecal->SetWorldLocation(HitResult.Location);
            }
        }
    }
}

