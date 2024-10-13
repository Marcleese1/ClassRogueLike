#include "Character/Player/MainPlayerCharacter.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "AI/PlayerAIController.h"
#include "Player/MainPlayerState.h"
#include "Character/Abilities/CharacterAbilitySystemComponent.h"
#include "Character/Abilities/CharacterGameplayAbility.h"  // Ensure this header is included
#include "Kismet/KismetMathLibrary.h"
#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"
#include "InputMappingContext.h"
#include "InputAction.h"
#include "GameplayEffect.h"
#include "BP_Enemy.h"

AMainPlayerCharacter::AMainPlayerCharacter(const FObjectInitializer& ObjectInitializer)
    : Super(ObjectInitializer)
{

   


    // Initialize input actions and contexts
    static ConstructorHelpers::FObjectFinder<UInputAction> UseAbilityActionFinder(TEXT("/Game/Input/IA_UseAbility.IA_UseAbility"));
    UseAbilityAction = UseAbilityActionFinder.Object;

    static ConstructorHelpers::FObjectFinder<UInputMappingContext> AbilityMappingContextFinder(TEXT("/Game/Input/IMC_AbilityMapping.IMC_AbilityMapping"));
    AbilityMappingContext = AbilityMappingContextFinder.Object;

    static ConstructorHelpers::FObjectFinder<UInputMappingContext> FighterAbilitiesMappingContextFinder(TEXT("/Game/Input/IMC_FighterAbilities.IMC_FighterAbilities"));
    FighterAbilitiesMappingContext = FighterAbilitiesMappingContextFinder.Object;

    static ConstructorHelpers::FObjectFinder<UInputAction> StartTargetingActionFinder(TEXT("/Game/Input/IA_StartTargeting.IA_StartTargeting"));
    StartTargetingAction = StartTargetingActionFinder.Object; 

    static ConstructorHelpers::FObjectFinder<UInputAction> ConfirmTargetActionFinder(TEXT("/Game/Input/IA_Confirm.IA_Confirm"));
    ConfirmTargetAction = ConfirmTargetActionFinder.Object;

    // Initialize camera components
    CameraBoom = CreateDefaultSubobject<USpringArmComponent>(FName("CameraBoom"));
    CameraBoom->SetupAttachment(RootComponent);
    CameraBoom->bUsePawnControlRotation = true;
    CameraBoom->SetRelativeLocation(FVector(0, 0, 70));

    FollowCamera = CreateDefaultSubobject<UCameraComponent>(FName("FollowCamera"));
    FollowCamera->SetupAttachment(CameraBoom);
    FollowCamera->FieldOfView = 90.0f;

    // Configure capsule and mesh collision
    GetCapsuleComponent()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);
    GetMesh()->VisibilityBasedAnimTickOption = EVisibilityBasedAnimTickOption::AlwaysTickPoseAndRefreshBones;
    GetMesh()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    GetMesh()->SetCollisionProfileName(FName("NoCollision"));

    // Set AI controller and gameplay tags
    AIControllerClass = APlayerAIController::StaticClass();
    DeadTag = FGameplayTag::RequestGameplayTag(FName("State.Dead"));

    // Load movement mapping context and actions
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

    // Initialize attack collision component
    AttackCollision = CreateDefaultSubobject<UCapsuleComponent>(TEXT("AttackCollision"));
    AttackCollision->SetupAttachment(GetMesh(), FName("WeaponSocket"));
    AttackCollision->SetCollisionProfileName(TEXT("OverlapAllDynamic"));
    AttackCollision->SetGenerateOverlapEvents(false);  // Disable by default
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

        EnhancedInputComponent->BindAction(UseAbilityAction, ETriggerEvent::Triggered, this, &AMainPlayerCharacter::HandleInputActionTriggered);
        EnhancedInputComponent->BindAction(UseAbilityAction, ETriggerEvent::Completed, this, &AMainPlayerCharacter::HandleInputActionCompleted);

        EnhancedInputComponent->BindAction(StartTargetingAction, ETriggerEvent::Triggered, this, &AMainPlayerCharacter::HandleInputActionTriggered);
        EnhancedInputComponent->BindAction(StartTargetingAction, ETriggerEvent::Completed, this, &AMainPlayerCharacter::HandleInputActionCompleted);

        EnhancedInputComponent->BindAction(ConfirmTargetAction, ETriggerEvent::Triggered, this, &AMainPlayerCharacter::HandleInputActionTriggered);
        EnhancedInputComponent->BindAction(ConfirmTargetAction, ETriggerEvent::Completed, this, &AMainPlayerCharacter::HandleInputActionCompleted);

        // Bind the attack action to activate attack
        EnhancedInputComponent->BindAction(UseAbilityAction, ETriggerEvent::Triggered, this, &AMainPlayerCharacter::ActivateAttack);
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
            Subsystem->AddMappingContext(FighterAbilitiesMappingContext, 1);
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

    if (!AbilitySystemComponent)
    {
        UE_LOG(LogTemp, Error, TEXT("AbilitySystemComponent is not initialized"));
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("AbilitySystemComponent initialized successfully"));
        BindASCInput();
    }

    // Bind overlap event
    AttackCollision->OnComponentBeginOverlap.AddDynamic(this, &AMainPlayerCharacter::OnAttackOverlapBegin);
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

void AMainPlayerCharacter::OnRep_PlayerState()
{
    Super::OnRep_PlayerState(); // Call the base class implementation
    if (!ASCInputBound)
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
    SetStamina(GetMaxStamina());
}

void AMainPlayerCharacter::BindASCInput()
{
    if (!ASCInputBound && AbilitySystemComponent && IsValid(InputComponent))
    {
        FTopLevelAssetPath AbilityEnumAssetPath = FTopLevelAssetPath(FName("/Script/ClassRoguelike"), FName("BaseAbilityID"));

        AbilitySystemComponent->BindAbilityActivationToInputComponent(InputComponent,
            FGameplayAbilityInputBinds(
                FString("ConfirmTarget"),
                FString("CancelTarget"),
                AbilityEnumAssetPath,
                static_cast<int32>(BaseAbilityID::Confirm),
                static_cast<int32>(BaseAbilityID::Cancel)
            )
        );

        UE_LOG(LogTemp, Warning, TEXT("Starting to bind abilities to input actions"));

        for (auto& Entry : InputToAbilityMap)
        {
            UInputAction* InputAction = const_cast<UInputAction*>(Entry.Key);
            FGameplayAbilitySpecHandle AbilityHandle = Entry.Value;

            if (!InputAction)
            {
                UE_LOG(LogTemp, Error, TEXT("InputAction is nullptr"));
                continue;
            }

            if (!AbilityHandle.IsValid())
            {
                UE_LOG(LogTemp, Error, TEXT("AbilityHandle is invalid"));
                continue;
            }

            if (AbilitySystemComponent)
            {
                AbilitySystemComponent->SetInputBinding(InputAction, AbilityHandle);
            }
            else
            {
                UE_LOG(LogTemp, Error, TEXT("AbilitySystemComponent is nullptr"));
            }
        }

        ASCInputBound = true;
        UE_LOG(LogTemp, Warning, TEXT("ASCInputBound successfully set to true"));
    }
}

void AMainPlayerCharacter::UseAbility(const FInputActionValue& Value)
{
    if (Value.Get<bool>())
    {
        UAbilitySystemComponent* ASC = GetAbilitySystemComponent();
        if (ASC)
        {
            for (const FGameplayAbilitySpec& Spec : ASC->GetActivatableAbilities())
            {
                if (Spec.InputID == static_cast<int32>(BaseAbilityID::Ability1))
                {
                    ASC->TryActivateAbility(Spec.Handle);
                    break;
                }
            }
        }
    }
}

void AMainPlayerCharacter::StartTargeting(const FInputActionValue& Value)
{
    UAbilitySystemComponent* ASC = GetAbilitySystemComponent();
    if (ASC)
    {
        FGameplayAbilitySpec* AbilitySpec = ASC->FindAbilitySpecFromInputID(static_cast<int32>(BaseAbilityID::Ability1));
        if (AbilitySpec)
        {
            ASC->TryActivateAbility(AbilitySpec->Handle);
        }
    }
}

void AMainPlayerCharacter::ConfirmTarget(const FInputActionValue& Value)
{
    UAbilitySystemComponent* ASC = GetAbilitySystemComponent();
    if (ASC)
    {
        for (const FGameplayAbilitySpec& Spec : ASC->GetActivatableAbilities())
        {
            if (UCharacterGameplayAbility* Ability = Cast<UCharacterGameplayAbility>(Spec.Ability))
            {
                if (Ability->IsActive())
                {
                    Ability->ConfirmTarget();
                    break;
                }
            }
        }
    }
}

void AMainPlayerCharacter::OnAttackOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
    UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
    /*if (OtherActor && OtherActor != this && OtherComp)
    {
        ABP_Enemy* Enemy = Cast<ABP_Enemy>(OtherActor);
        if (Enemy)
        {
            FGameplayEffectSpecHandle DamageEffectSpec = AbilitySystemComponent->MakeOutgoingSpec(
                USlashDamageGameplayEffect::StaticClass(), GetCharacterLevel(), AbilitySystemComponent->MakeEffectContext());

            if (DamageEffectSpec.IsValid())
            {
                AbilitySystemComponent->ApplyGameplayEffectSpecToTarget(*DamageEffectSpec.Data.Get(), Enemy->GetAbilitySystemComponent());
                UE_LOG(LogTemp, Warning, TEXT("Enemy hit with existing slash damage effect"));
            }
        }
    }*/
}


void AMainPlayerCharacter::ActivateAttack()
{
    AttackCollision->SetGenerateOverlapEvents(true);

    GetWorldTimerManager().SetTimer(AttackTimerHandle, this, &AMainPlayerCharacter::DeactivateAttack, 0.5f, false);
}

void AMainPlayerCharacter::DeactivateAttack()
{
    AttackCollision->SetGenerateOverlapEvents(false);
}

void AMainPlayerCharacter::HandleInputActionTriggered(const FInputActionInstance& ActionInstance)
{
    UCharacterAbilitySystemComponent* ASC = Cast<UCharacterAbilitySystemComponent>(GetAbilitySystemComponent());
    if (ASC)
    {
        ASC->ProcessInputAction(ActionInstance.GetSourceAction(), true);
    }
}

void AMainPlayerCharacter::HandleInputActionCompleted(const FInputActionInstance& ActionInstance)
{
    UCharacterAbilitySystemComponent* ASC = Cast<UCharacterAbilitySystemComponent>(GetAbilitySystemComponent());
    if (ASC)
    {
        ASC->ProcessInputAction(ActionInstance.GetSourceAction(), false);
    }
}
