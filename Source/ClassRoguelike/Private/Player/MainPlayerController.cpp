 // Fill out your copyright notice in the Description page of Project Settings.


#include "Player/MainPlayerController.h"
#include "Player/MainPlayerState.h"
#include "AbilitySystemComponent.h"
#include "EnhancedInputSubsystems.h"
#include <Character/Player/MainPlayerCharacter.h>

void AMainPlayerController::OnPossess(APawn* InPawn)
{
    Super::OnPossess(InPawn);

    AMainPlayerState* PS = GetPlayerState<AMainPlayerState>();
    if (PS)
    {
        PS->GetAbilitySystemComponent()->InitAbilityActorInfo(PS, InPawn);
    }

    // Rename 'Character' to 'PossessedCharacter' or another unique name
    if (AMainPlayerCharacter* PossessedCharacter = Cast<AMainPlayerCharacter>(InPawn))
    {
        if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
        {
            Subsystem->AddMappingContext(PossessedCharacter->MovementMappingContext, 1);
        }
    }
}

//TODO -- Add Hud Stuff