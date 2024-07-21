#include "BP_Enemy.h"
#include "AbilitySystemComponent.h"
#include "Enemy/AttributeSet/EnemyAttributeSet.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/WidgetComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"
#include "Enemy/UI/BP_EnemyHealthBarWidget.h"

DEFINE_LOG_CATEGORY_STATIC(LogBP_Enemy, Log, All);

ABP_Enemy::ABP_Enemy(const class FObjectInitializer& ObjectInitializer) : ACharacterBase(ObjectInitializer)
{
    PrimaryActorTick.bCanEverTick = true;

    AbilitySystemComponent = CreateDefaultSubobject<UCharacterAbilitySystemComponent>(TEXT("AbilitySystemComponent"));
    AbilitySystemComponent->SetIsReplicated(true);
    AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Minimal);

    EnemyAttributeSetBase = CreateDefaultSubobject<UEnemyAttributeSet>(TEXT("AttributeSet"));

    UIFloatingStatusBarComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("UIFloatingStatusBarComponent"));
    UIFloatingStatusBarComponent->SetupAttachment(RootComponent);

    UIFloatingStatusBarClass = StaticLoadClass(UObject::StaticClass(), nullptr, TEXT("/Game/ClassRogueLike/Enemies/WB_EnemyHealthbar.WB_EnemyHealthbar_C"));
    if (!UIFloatingStatusBarClass)
    {
        UE_LOG(LogBP_Enemy, Error, TEXT("%s() Failed to find UIFloatingStatusBarClass. If it was moved, please update the reference location in C++."), *FString(__FUNCTION__));
    }

    DeadTag = FGameplayTag::RequestGameplayTag(FName("State.Dead"));
    EffectRemoveOnDeathTag = FGameplayTag::RequestGameplayTag(FName("Effect.RemoveOnDeath"));
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
                    UIFloatingStatusBarComponent->SetDrawSize(FVector2D(500, 500));
                    UpdateHealthBar();
                }
            }
        }

        HealthChangedDelegateHandle = AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(EnemyAttributeSetBase->GetHealthAttribute()).AddUObject(this, &ABP_Enemy::HealthChanged);
    }
}

void ABP_Enemy::UpdateHealthBar()
{
    if (UIFloatingStatusBar)
    {
        float HealthPercentage = GetHealth() / GetMaxHealth();
        HealthPercentage = FMath::Clamp(HealthPercentage, 0.0f, 1.0f);
        UIFloatingStatusBar->SetHealthPercentage(HealthPercentage);
    }
}

void ABP_Enemy::HealthChanged(const FOnAttributeChangeData& Data)
{
    float Health = Data.NewValue;
    UE_LOG(LogBP_Enemy, Log, TEXT("Health Changed: %f"), Health);
    UpdateHealthBar();

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

    /*OnEnemyDied.Broadcast(this);*/

    if (AbilitySystemComponent)
    {
        AbilitySystemComponent->CancelAllAbilities();

        FGameplayTagContainer EffectsTagsToRemove;
        EffectsTagsToRemove.AddTag(EffectRemoveOnDeathTag);
        int32 NumEffectsRemoved = AbilitySystemComponent->RemoveActiveEffectsWithTags(EffectsTagsToRemove);

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

UAbilitySystemComponent* ABP_Enemy::GetAbilitySystemComponent() const
{
    return AbilitySystemComponent;
}

void ABP_Enemy::RemoveCharacterAbilities()
{
    if (GetLocalRole() != ROLE_Authority || !AbilitySystemComponent)
    {
        return;
    }

    TArray<FGameplayAbilitySpecHandle> AbilitiesToRemove;
    for (const FGameplayAbilitySpec& Spec : AbilitySystemComponent->GetActivatableAbilities())
    {
        AbilitiesToRemove.Add(Spec.Handle);
    }

    for (int32 i = 0; i < AbilitiesToRemove.Num(); i++)
    {
        AbilitySystemComponent->ClearAbility(AbilitiesToRemove[i]);
    }
}

