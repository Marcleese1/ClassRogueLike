#include "BP_Enemy.h"
#include "Character/Abilities/CharacterAbilitySystemComponent.h"
#include "Character/Abilities/AttributeSets/CharacterAttributeSetBase.h"
#include "Components/WidgetComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Enemy/UI/BP_EnemyHealthBarWidget.h"
#include "Net/UnrealNetwork.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/CapsuleComponent.h"

DEFINE_LOG_CATEGORY_STATIC(LogBP_Enemy, Log, All);

ABP_Enemy::ABP_Enemy(const class FObjectInitializer& ObjectInitializer) : ACharacterBase(ObjectInitializer)
{
    PrimaryActorTick.bCanEverTick = true;  // Enable ticking

    UCapsuleComponent* PunchCollision = CreateDefaultSubobject<UCapsuleComponent>(TEXT("PunchCollision"));
    PunchCollision->SetupAttachment(RootComponent);
    PunchCollision->SetCollisionProfileName(TEXT("OverlapAllDynamic"));

    // Initialize the Ability System Component
    if (!AbilitySystemComponent) {
        AbilitySystemComponent = CreateDefaultSubobject<UCharacterAbilitySystemComponent>(TEXT("AbilitySystemComponent"));
        AbilitySystemComponent->SetIsReplicated(true);
        AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Minimal);
    }

    // Initialize the Attribute Set
    if (!AttributeSetBase) {
        AttributeSetBase = CreateDefaultSubobject<UCharacterAttributeSetBase>(TEXT("AttributeSetBase"));
    }

    // Initialize UI Component
    UIFloatingStatusBarComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("UIFloatingStatusBarComponent"));
    UIFloatingStatusBarComponent->SetupAttachment(RootComponent);
    UIFloatingStatusBarComponent->SetRelativeLocation(FVector(0, 0, 120)); // Position above the enemy
    UIFloatingStatusBarComponent->SetWidgetSpace(EWidgetSpace::World);
    UIFloatingStatusBarComponent->SetDrawSize(FVector2D(100, 1001));

    // Load the Floating Status Bar Class
    UIFloatingStatusBarClass = StaticLoadClass(UObject::StaticClass(), nullptr, TEXT("/Game/ClassRogueLike/Enemies/WB_EnemyHealthbar.WB_EnemyHealthbar_C"));
    if (!UIFloatingStatusBarClass)
    {
        UE_LOG(LogBP_Enemy, Error, TEXT("%s() Failed to find UIFloatingStatusBarClass. If it was moved, please update the reference location in C++."), *FString(__FUNCTION__));
    }

    // Initialize rotation variables
    RotationSpeed = 5.0f;  // Adjust rotation speed as needed
    TargetRotation = FRotator::ZeroRotator;
}

void ABP_Enemy::BeginPlay()
{
    Super::BeginPlay();

    if (AbilitySystemComponent)
    {
        AbilitySystemComponent->InitAbilityActorInfo(this, this);
        InitializeAttributes();
        AddStartupEffects();

        APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0);
        if (PC && PC->IsLocalPlayerController())
        {
            if (UIFloatingStatusBarClass)
            {
                UIFloatingStatusBar = CreateWidget<UBP_EnemyHealthBarWidget>(PC, UIFloatingStatusBarClass);
                if (UIFloatingStatusBar && UIFloatingStatusBarComponent)
                {
                    UIFloatingStatusBarComponent->SetWidget(UIFloatingStatusBar);
                    float HealthPercentage = GetHealth() / GetMaxHealth();
                    UE_LOG(LogTemp, Warning, TEXT("Setting Health Percentage: %f"), HealthPercentage);
                    HealthPercentage = FMath::Clamp(HealthPercentage, 0.0f, 1.0f);  // Ensure it’s clamped
                    UIFloatingStatusBar->SetHealthPercentage(HealthPercentage);
                }
            }
        }

        // Bind health change
        HealthChangedDelegateHandle = AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(
            AttributeSetBase->GetHealthAttribute()).AddUObject(this, &ABP_Enemy::OnHealthChanged);
    }
}

void ABP_Enemy::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    // Update target rotation based on logic (e.g., player position or direction)
    UpdateTargetRotation();

    // Smoothly interpolate to the target rotation
    FRotator CurrentRotation = GetActorRotation();
    FRotator NewRotation = FMath::RInterpTo(CurrentRotation, TargetRotation, DeltaTime, RotationSpeed);
    SetActorRotation(NewRotation);
}

void ABP_Enemy::UpdateTargetRotation()
{
    // Example: Rotate towards the player
    APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0);
    if (PC)
    {
        APawn* PlayerPawn = PC->GetPawn();
        if (PlayerPawn)
        {
            FVector DirectionToPlayer = PlayerPawn->GetActorLocation() - GetActorLocation();
            DirectionToPlayer.Z = 0;  // Ignore vertical difference
            TargetRotation = DirectionToPlayer.Rotation();
        }
    }
}

void ABP_Enemy::OnHealthChanged(const FOnAttributeChangeData& Data)
{
    float NewHealth = Data.NewValue;

    UE_LOG(LogTemp, Log, TEXT("Health Changed: %f"), NewHealth);

    // Update floating status bar
    if (UIFloatingStatusBar)
    {
        UIFloatingStatusBar->SetHealthPercentage(NewHealth / GetMaxHealth());
    }

    // Directly handle health change logic
    // Update the health bar...
    if (NewHealth <= 0.0f && !AbilitySystemComponent->HasMatchingGameplayTag(DeadTag))
    {
        Die();
    }
}

void ABP_Enemy::Die()
{
    // Custom Die logic if needed, then call the base class Die method
    Super::Die();
}

//void ABP_Enemy::Die()
//{
//    // Exit early if the object is invalid or already marked as dead.
//    if (!IsValid(this) || AbilitySystemComponent->HasMatchingGameplayTag(DeadTag))
//    {
//        return;
//    }
//
//    // Broadcast death event so other systems can stop referencing this actor
//    OnCharacterDied.Broadcast(this);
//
//    RemoveCharacterAbilities();
//    GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
//    GetCharacterMovement()->GravityScale = 0;
//    GetCharacterMovement()->Velocity = FVector(0);
//
//    if (AbilitySystemComponent)
//    {
//        AbilitySystemComponent->CancelAllAbilities();
//
//        FGameplayTagContainer EffectsTagsToRemove;
//        EffectsTagsToRemove.AddTag(EffectRemoveOnDeathTag);
//        AbilitySystemComponent->RemoveActiveEffectsWithTags(EffectsTagsToRemove);
//
//        AbilitySystemComponent->AddLooseGameplayTag(DeadTag);
//    }
//
//    if (DeathMontage)
//    {
//        // Use a timer to delay the destruction, ensuring no immediate issues with references.
//        GetWorld()->GetTimerManager().SetTimerForNextTick(this, &ABP_Enemy::FinishDying);
//        PlayAnimMontage(DeathMontage);
//    }
//    else
//    {
//        FinishDying();
//    }
//}

//bool ABP_Enemy::IsEnemyValid() const
//{
//    return IsValid(this) && !AbilitySystemComponent->HasMatchingGameplayTag(DeadTag);
//}

void ABP_Enemy::FinishDying()
{
    // Set a lifespan for the actor before it's actually destroyed to allow any ongoing logic to complete.
    SetLifeSpan(0.1f);
}
