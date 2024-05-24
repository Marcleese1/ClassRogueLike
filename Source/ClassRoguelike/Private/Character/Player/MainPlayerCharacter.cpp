#include "Character/Player/MainPlayerCharacter.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "AI/PlayerAIController.h"
#include "Player/MainPlayerState.h"
#include "Character/Abilities/CharacterAbilitySystemComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"
#include "InputMappingContext.h"
#include "InputAction.h"

AMainPlayerCharacter::AMainPlayerCharacter(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
    static ConstructorHelpers::FObjectFinder<UInputAction> UseAbilityActionFinder(TEXT("/Game/Input/IA_UseAbility.IA_UseAbility"));
    UseAbilityAction = UseAbilityActionFinder.Object; // UPDATED

    static ConstructorHelpers::FObjectFinder<UInputMappingContext> AbilityMappingContextFinder(TEXT("/Game/Input/IMC_AbilityMapping.IMC_AbilityMapping"));
    AbilityMappingContext = AbilityMappingContextFinder.Object; // UPDATED

    CameraBoom = CreateDefaultSubobject<USpringArmComponent>(FName("CameraBoom"));
    CameraBoom->SetupAttachment(RootComponent);
    CameraBoom->bUsePawnControlRotation = true;
    CameraBoom->SetRelativeLocation(FVector(0, 0, 70));

    FollowCamera = CreateDefaultSubobject<UCameraComponent>(FName("FollowCamera"));
    FollowCamera->SetupAttachment(CameraBoom);
    FollowCamera->FieldOfView = 90.0f;

    GetCapsuleComponent()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);
    GetMesh()->VisibilityBasedAnimTickOption = EVisibilityBasedAnimTickOption::AlwaysTickPoseAndRefreshBones;
    GetMesh()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    GetMesh()->SetCollisionProfileName(FName("NoCollision"));

    AIControllerClass = APlayerAIController::StaticClass();
    DeadTag = FGameplayTag::RequestGameplayTag(FName("State.Dead"));

    // Load input mapping context and actions
    static ConstructorHelpers::FObjectFinder<UInputMappingContext> MovementContextFinder(TEXT("/Game/ClassRoguelike/Characters/InputActions/IMC_Movement.IMC_Movement"));
    MovementMappingContext = MovementContextFinder.Object;

    static ConstructorHelpers::FObjectFinder<UInputAction> LookUpActionFinder(TEXT("/Game/ClassRoguelike/Characters/InputActions/IA_LookUp.IA_LookUp"));
    LookUpAction = LookUpActionFinder.Object;

    static ConstructorHelpers::FObjectFinder<UInputAction> LookUpRateActionFinder(TEXT("/Game/ClassRoguelike/Characters/InputActions/IA_LookUpRate.IA_LookUpRate"));
    LookUpRateAction = LookUpRateActionFinder.Object;

    static ConstructorHelpers::FObjectFinder<UInputAction> TurnActionFinder(TEXT("/Game/ClassRoguelike/Characters/InputActions/IA_Turn.IA_Turn"));
    TurnAction = TurnActionFinder.Object;

    static ConstructorHelpers::FObjectFinder<UInputAction> TurnRateActionFinder(TEXT("/Game/ClassRoguelike/Characters/InputActions/IA_TurnRate.IA_TurnRate"));
    TurnRateAction = TurnRateActionFinder.Object;

    static ConstructorHelpers::FObjectFinder<UInputAction> MoveForwardActionFinder(TEXT("/Game/ClassRoguelike/Characters/InputActions/IA_MoveForward.IA_MoveForward"));
    MoveForwardAction = MoveForwardActionFinder.Object;

    static ConstructorHelpers::FObjectFinder<UInputAction> MoveRightActionFinder(TEXT("/Game/ClassRoguelike/Characters/InputActions/IA_MoveRight.IA_MoveRight"));
    MoveRightAction = MoveRightActionFinder.Object;

    if (MovementMappingContext)
    {
        UE_LOG(LogTemp, Warning, TEXT("MovementMappingContext loaded successfully"));
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("Failed to load MovementMappingContext"));
    }

    if (LookUpAction)
    {
        UE_LOG(LogTemp, Warning, TEXT("LookUpAction loaded successfully"));
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("Failed to load LookUpAction"));
    }
}

void AMainPlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
    Super::SetupPlayerInputComponent(PlayerInputComponent);
    if (UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent))
    {
        EnhancedInputComponent->BindAction(LookUpAction, ETriggerEvent::Triggered, this, &AMainPlayerCharacter::LookUp);
        EnhancedInputComponent->BindAction(LookUpRateAction, ETriggerEvent::Triggered, this, &AMainPlayerCharacter::LookUpRate);
        EnhancedInputComponent->BindAction(TurnAction, ETriggerEvent::Triggered, this, &AMainPlayerCharacter::Turn);
        EnhancedInputComponent->BindAction(TurnRateAction, ETriggerEvent::Triggered, this, &AMainPlayerCharacter::TurnRate);
        EnhancedInputComponent->BindAction(MoveForwardAction, ETriggerEvent::Triggered, this, &AMainPlayerCharacter::MoveForward);
        EnhancedInputComponent->BindAction(MoveRightAction, ETriggerEvent::Triggered, this, &AMainPlayerCharacter::MoveRight);
        EnhancedInputComponent->BindAction(UseAbilityAction, ETriggerEvent::Started, this, &AMainPlayerCharacter::UseAbility);
    }

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

    // Add input mapping context
    if (APlayerController* PC = Cast<APlayerController>(NewController))
    {
        if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PC->GetLocalPlayer()))
        {
            Subsystem->AddMappingContext(MovementMappingContext, 1);
            Subsystem->AddMappingContext(AbilityMappingContext, 1);
        }
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

void AMainPlayerCharacter::LookUp(const FInputActionValue& Value)
{
    if (IsAlive())
    {
        AddControllerPitchInput(Value.Get<float>());
    }
}

void AMainPlayerCharacter::LookUpRate(const FInputActionValue& Value)
{
    if (IsAlive())
    {
        AddControllerPitchInput(Value.Get<float>() * BaseLookupRate * GetWorld()->DeltaTimeSeconds);
    }
}

void AMainPlayerCharacter::Turn(const FInputActionValue& Value)
{
    if (IsAlive())
    {
        UE_LOG(LogTemp, Warning, TEXT("Turn: %f"), Value.Get<float>());
        AddControllerYawInput(Value.Get<float>());
    }
}

void AMainPlayerCharacter::TurnRate(const FInputActionValue& Value)
{
    if (IsAlive())
    {
        AddControllerYawInput(Value.Get<float>() * BaseTurnRate * GetWorld()->DeltaTimeSeconds);
    }
}

void AMainPlayerCharacter::MoveForward(const FInputActionValue& Value)
{
    if (IsAlive())
    {
        UE_LOG(LogTemp, Warning, TEXT("MoveForward: %f"), Value.Get<float>());
        AddMovementInput(UKismetMathLibrary::GetForwardVector(FRotator(0, GetControlRotation().Yaw, 0)), Value.Get<float>());
    }
}

void AMainPlayerCharacter::MoveRight(const FInputActionValue& Value)
{
    if (IsAlive())
    {
        AddMovementInput(UKismetMathLibrary::GetRightVector(FRotator(0, GetControlRotation().Yaw, 0)), Value.Get<float>());
    }
}

void AMainPlayerCharacter::OnRep_PlayerState() // Ensure input bindings are refreshed when player state is replicated
{
    Super::OnRep_PlayerState();
    if (ASCInputBound)
    {
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

void AMainPlayerCharacter::UseAbility(const FInputActionValue& Value)
{
    if (Value.Get<bool>())
    {
        UE_LOG(LogTemp, Warning, TEXT("Ability Used!"));
        // Implement your ability logic here
    }
}