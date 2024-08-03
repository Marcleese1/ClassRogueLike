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
    PrimaryActorTick.bCanEverTick = false;

    // Initialize the Ability System Component
    if(!AbilitySystemComponent){
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

                    // Debug log
                    
                }
            }
        }
    }
}




void ABP_Enemy::OnHealthChanged(const FOnAttributeChangeData& Data)
{
    float NewHealth = Data.NewValue;

    UE_LOG(LogTemp, Log, TEXT("Health Changed: %f"), NewHealth);

    // Directly handle health change logic
    if (!IsAlive() && !AbilitySystemComponent->HasMatchingGameplayTag(DeadTag))
    {
        Die();
    }
}

void ABP_Enemy::Die()
{
    RemoveCharacterAbilities();
    GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    GetCharacterMovement()->GravityScale = 0;
    GetCharacterMovement()->Velocity = FVector(0);

    if (AbilitySystemComponent)
    {
        AbilitySystemComponent->CancelAllAbilities();

        FGameplayTagContainer EffectsTagsToRemove;
        EffectsTagsToRemove.AddTag(EffectRemoveOnDeathTag);
        AbilitySystemComponent->RemoveActiveEffectsWithTags(EffectsTagsToRemove);

        AbilitySystemComponent->AddLooseGameplayTag(DeadTag);
    }

    if (DeathMontage)
    {
        PlayAnimMontage(DeathMontage);
    }
    else
    {
        FinishDying();
    }
}

void ABP_Enemy::FinishDying()
{
    Destroy();
}