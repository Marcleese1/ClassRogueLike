#include "BP_Enemy.h"
#include "Enemy/AttributeSet/EnemyAttributeSet.h"
#include "GameplayEffect.h"
#include "Components/WidgetComponent.h"
#include "Enemy/UI/BP_EnemyHealthBarWidget.h"
#include "Net/UnrealNetwork.h"
#include "Kismet/GameplayStatics.h"
#include "Enemy/EnemyAbilitySystemComponent.h"

DEFINE_LOG_CATEGORY_STATIC(LogBP_Enemy, Log, All);

ABP_Enemy::ABP_Enemy(const FObjectInitializer& ObjectInitializer)
    : Super(ObjectInitializer), InitialHealth(100.0f), InitialMaxHealth(100.0f)
{
    // Create Ability System Component
    EnemyAbilitySystemComponent = CreateDefaultSubobject<UEnemyAbilitySystemComponent>(TEXT("EnemyAbilitySystemComponent"));
    EnemyAbilitySystemComponent->SetIsReplicated(true);

    // Create Attribute Set (updated to use UEnemyAttributeSet)
    EnemyAttributeSet = CreateDefaultSubobject<UEnemyAttributeSet>(TEXT("EnemyAttributeSet"));
    if (EnemyAbilitySystemComponent && EnemyAttributeSet)
    {
        EnemyAbilitySystemComponent->AddAttributeSetSubobject(EnemyAttributeSet);
        UE_LOG(LogBP_Enemy, Warning, TEXT("Attribute Set Registered: %s"), *EnemyAttributeSet->GetName());
    }

    // Create Health Bar UI (no changes required here)
    UIFloatingStatusBarComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("UIFloatingStatusBarComponent"));
    UIFloatingStatusBarComponent->SetupAttachment(RootComponent);
    UIFloatingStatusBarComponent->SetRelativeLocation(FVector(0, 0, 120));
    UIFloatingStatusBarComponent->SetWidgetSpace(EWidgetSpace::World);
    UIFloatingStatusBarComponent->SetDrawSize(FVector2D(100, 100));

    UIFloatingStatusBarClass = StaticLoadClass(UObject::StaticClass(), nullptr, TEXT("/Game/ClassRogueLike/Enemies/WB_EnemyHealthbar.WB_EnemyHealthbar_C"));
    if (!UIFloatingStatusBarClass)
    {
        UE_LOG(LogBP_Enemy, Error, TEXT("Failed to load UIFloatingStatusBarClass."));
    }

    RotationSpeed = 5.0f;
    TargetRotation = FRotator::ZeroRotator;
}

void ABP_Enemy::BeginPlay()
{
    Super::BeginPlay();

    if (!EnemyAbilitySystemComponent || !EnemyAttributeSet)
    {
        UE_LOG(LogBP_Enemy, Error, TEXT("Ability System or Attribute Set is null in %s"), *GetName());
        return;
    }

    if (GetLocalRole() == ROLE_Authority)
    {
        InitializeAttributes();
        AddStartupEffects();
    }

    // Setup health and max health values from the applied GameplayEffect
    HealthChangedDelegateHandle = EnemyAbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(
        EnemyAttributeSet->GetHealthAttribute()).AddUObject(this, &ABP_Enemy::OnHealthChanged);

    MaxHealthChangedDelegateHandle = EnemyAbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(
        EnemyAttributeSet->GetMaxHealthAttribute()).AddUObject(this, &ABP_Enemy::OnMaxHealthChangedDelegate);

    SetHealth(GetMaxHealth());

    if (UIFloatingStatusBarClass)
    {
        APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0);
        if (PC && PC->IsLocalPlayerController())
        {
            UIFloatingStatusBar = CreateWidget<UBP_EnemyHealthBarWidget>(PC, UIFloatingStatusBarClass);
            if (UIFloatingStatusBar && UIFloatingStatusBarComponent)
            {
                UIFloatingStatusBarComponent->SetWidget(UIFloatingStatusBar);
                UpdateHealthBar();
            }
        }
    }
}

UAbilitySystemComponent* ABP_Enemy::GetAbilitySystemComponent() const
{
    return EnemyAbilitySystemComponent;
}

void ABP_Enemy::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    // Update the target rotation based on the enemy's movement direction
    UpdateTargetRotation();

    // Rotate towards the direction they're moving
    FRotator NewRotation = FMath::RInterpTo(GetActorRotation(), TargetRotation, DeltaTime, RotationSpeed);
    SetActorRotation(NewRotation);
}

void ABP_Enemy::UpdateTargetRotation()
{
    // Get the enemy's current velocity
    FVector Velocity = GetVelocity();

    // Only update rotation if the enemy is moving
    if (!Velocity.IsNearlyZero())
    {
        // Get the direction of the velocity, ignore Z to only rotate on the horizontal plane
        FVector Direction = Velocity.GetSafeNormal2D();
        TargetRotation = Direction.Rotation();
    }
}

void ABP_Enemy::InitializeAttributes()
{
    UE_LOG(LogBP_Enemy, Warning, TEXT("===== InitializeAttributes START for %s ====="), *GetName());

    if (!EnemyAbilitySystemComponent || !DefaultAttributesForEnemies) return;

    FGameplayEffectContextHandle EffectContext = EnemyAbilitySystemComponent->MakeEffectContext();
    EffectContext.AddSourceObject(this);

    FGameplayEffectSpecHandle SpecHandle = EnemyAbilitySystemComponent->MakeOutgoingSpec(DefaultAttributesForEnemies, GetCharacterLevel(), EffectContext);

    if (SpecHandle.IsValid())
    {
        FActiveGameplayEffectHandle GEHandle = EnemyAbilitySystemComponent->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());

        // Log to verify that the GameplayEffect is applied correctly
        UE_LOG(LogBP_Enemy, Warning, TEXT("Applied DefaultAttributesForEnemies effect"));
    }

    UE_LOG(LogBP_Enemy, Warning, TEXT("===== InitializeAttributes END for %s ====="), *GetName());
}

void ABP_Enemy::AddStartupEffects()
{
    if (GetLocalRole() != ROLE_Authority || !EnemyAbilitySystemComponent || EnemyAbilitySystemComponent->StartupEffectsApplied)
    {
        return;
    }

    FGameplayEffectContextHandle EffectContext = EnemyAbilitySystemComponent->MakeEffectContext();
    EffectContext.AddSourceObject(this);

    for (TSubclassOf<UGameplayEffect> GameplayEffect : EnemyStartupEffects)
    {
        FGameplayEffectSpecHandle SpecHandle = EnemyAbilitySystemComponent->MakeOutgoingSpec(GameplayEffect, GetCharacterLevel(), EffectContext);
        if (SpecHandle.IsValid())
        {
            EnemyAbilitySystemComponent->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
            UE_LOG(LogBP_Enemy, Warning, TEXT("Enemy StartupEffect: %s applied"), *GameplayEffect->GetName());
        }
    }

    EnemyAbilitySystemComponent->StartupEffectsApplied = true;
}

void ABP_Enemy::OnHealthChanged(const FOnAttributeChangeData& Data)
{
    float NewHealth = Data.NewValue;
    UpdateHealthBar();

    if (NewHealth <= 0.0f)
    {
        Die();  // Call death logic if health reaches 0
    }
}



void ABP_Enemy::OnMaxHealthChangedDelegate(const FOnAttributeChangeData& Data) {}

void ABP_Enemy::UpdateHealthBar()
{
    if (UIFloatingStatusBar)
    {
        float Health = GetHealth();
        float MaxHealth = GetMaxHealth();

        if (MaxHealth > 0)
        {
            float HealthPercentage = FMath::Clamp(Health / MaxHealth, 0.0f, 1.0f);
            UIFloatingStatusBar->SetHealthPercentage(HealthPercentage);
        }
    }
}

void ABP_Enemy::Die()
{
    FinishDying();
}

void ABP_Enemy::FinishDying()
{
    UE_LOG(LogTemp, Warning, TEXT("%s is dying"), *GetName());
    SetLifeSpan(0.1f);
}

void ABP_Enemy::SetHealth(float HealthValue)
{
    if (EnemyAttributeSet)
    {
        HealthValue = FMath::Clamp(HealthValue, 0.0f, GetMaxHealth());
        EnemyAttributeSet->SetHealth(HealthValue);
        UpdateHealthBar();
    }
}

float ABP_Enemy::GetMaxHealth() const
{
    return EnemyAttributeSet ? EnemyAttributeSet->GetMaxHealth() : 0.0f;
}

float ABP_Enemy::GetHealth() const
{
    return EnemyAttributeSet ? EnemyAttributeSet->GetHealth() : 0.0f;
}

