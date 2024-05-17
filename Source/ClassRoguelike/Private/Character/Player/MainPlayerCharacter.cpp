// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/Player/MainPlayerCharacter.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "AI/PlayerAIController.h"
#include "Player/MainPlayerState.h"
#include "Character/Player/MainPlayerCharacter.h"
#include "Player/MainPlayerController.h"
#include "Character/Abilities/CharacterAbilitySystemComponent.h"
#include "Kismet/KismetMathLibrary.h"

AMainPlayerCharacter::AMainPlayerCharacter(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(FName("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->bUsePawnControlRotation = true;
	CameraBoom->SetRelativeLocation(FVector(0,0,70));

	FollowCamera = CreateDefaultSubobject<UCameraComponent>(FName("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom);
	FollowCamera->FieldOfView = 90.0f;

	GetCapsuleComponent()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);

	GetMesh()->VisibilityBasedAnimTickOption = EVisibilityBasedAnimTickOption::AlwaysTickPoseAndRefreshBones;
	GetMesh()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	GetMesh()->SetCollisionProfileName(FName("NoCollision"));

	AIControllerClass = APlayerAIController::StaticClass();

	DeadTag = FGameplayTag::RequestGameplayTag(FName("State.Dead"));
}

void AMainPlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAxis("MoveForward", this, &AMainPlayerCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &AMainPlayerCharacter::MoveRight);
	PlayerInputComponent->BindAxis("LookUp", this, &AMainPlayerCharacter::LookUp);
	PlayerInputComponent->BindAxis("LookUpRate", this, &AMainPlayerCharacter::LookUpRate);
	PlayerInputComponent->BindAxis("Turn", this, &AMainPlayerCharacter::Turn);
	PlayerInputComponent->BindAxis("TurnRate", this, &AMainPlayerCharacter::TurnRate);



	BindASCInput();

}

void AMainPlayerCharacter::PossessedBy(AController* NewController)
{

	Super::PossessedBy(NewController);

	AMainPlayerState* PS = GetPlayerState<AMainPlayerState>();

	if (PS)
	{
		InitializeStartingValues(PS);

		AddStartupEffects();
		AddCharacterAbilities();
	}
}

USpringArmComponent* AMainPlayerCharacter::GetCameraBoom()
{
	return CameraBoom;
}

UCameraComponent* AMainPlayerCharacter::GetFollowCamera()
{
	return FollowCamera;
}

float AMainPlayerCharacter::GetStartingCameraBoomArmLength()
{
	return StartingCameraBoomArmLength;
}

FVector AMainPlayerCharacter::GetStartingCameraBoomLocation()
{
	return StartingCameraBoomLocation;
}

void AMainPlayerCharacter::BeginPlay()
{
	Super::BeginPlay();

	StartingCameraBoomArmLength = CameraBoom->TargetArmLength;
	StartingCameraBoomLocation = CameraBoom->GetRelativeLocation();


}

void AMainPlayerCharacter::LookUp(float Value)
{
	if (IsAlive())
	{
		AddControllerPitchInput(Value);
	}
}

void AMainPlayerCharacter::LookUpRate(float Value)
{

	if (IsAlive())
	{
		AddControllerPitchInput(Value * BaseLookupRate * GetWorld()->DeltaTimeSeconds);
	}

}

void AMainPlayerCharacter::Turn(float Value)
{
	if (IsAlive())
	{
		AddControllerYawInput(Value);
	}
}

void AMainPlayerCharacter::TurnRate(float Value)
{
	if (IsAlive())
	{
		AddControllerYawInput(Value * BaseTurnRate * GetWorld()->DeltaTimeSeconds);
	}
}

void AMainPlayerCharacter::MoveForward(float Value)
{
	if (IsAlive())
	{
		AddMovementInput(UKismetMathLibrary::GetForwardVector(FRotator(0, GetControlRotation().Yaw, 0)), Value);
	}
}

void AMainPlayerCharacter::MoveRight(float Value)
{
	if (IsAlive())
	{
		AddMovementInput(UKismetMathLibrary::GetRightVector(FRotator(0, GetControlRotation().Yaw, 0)), Value);

	} 
}

void AMainPlayerCharacter::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();

	AMainPlayerState* PS = GetPlayerState<AMainPlayerState>();

	if (PS) 
	{

		InitializeStartingValues(PS);
		BindASCInput();
	}
}

void AMainPlayerCharacter::InitializeStartingValues(AMainPlayerState* PS)
{
		AbilitySystemComponent = Cast<UCharacterAbilitySystemComponent>(PS->GetAbilitySystemComponent());

		PS->GetAbilitySystemComponent()->InitAbilityActorInfo(PS, this);
		AttributeSetBase = PS->GetAttributeSetBase();
		AbilitySystemComponent->SetTagMapCount(DeadTag, 0);
		InitializeAttributes();
		SetHealth(GetMaxHealth());
		SetMana(GetMaxMana());
	
}

void AMainPlayerCharacter::BindASCInput()
{
	if (!ASCInputBound && AbilitySystemComponent.IsValid() && IsValid(InputComponent))
	{
		AbilitySystemComponent->BindAbilityActivationToInputComponent(InputComponent, FGameplayAbilityInputBinds(FString("ConfirmTarget"), FString("CancelTarget"), FString("BaseAbilityID"), static_cast<int32>(BaseAbilityID::Confirm), static_cast<int32>(BaseAbilityID::Cancel)));
		ASCInputBound = true;
	}
}
