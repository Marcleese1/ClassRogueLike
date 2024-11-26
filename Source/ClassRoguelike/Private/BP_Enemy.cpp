#include "BP_Enemy.h"
#include "Enemy/AttributeSet/EnemyAttributeSet.h"
#include "GameplayEffect.h"
#include "Components/WidgetComponent.h"
#include "Enemy/UI/BP_EnemyHealthBarWidget.h"
#include "Net/UnrealNetwork.h"
#include "Kismet/GameplayStatics.h"
#include "Enemy/EnemyAbilitySystemComponent.h"
#include "GameFramework/CharacterMovementComponent.h" 

DEFINE_LOG_CATEGORY_STATIC(LogBP_Enemy, Log, All);

ABP_Enemy::ABP_Enemy(const FObjectInitializer& ObjectInitializer)
    : Super(ObjectInitializer)
{
    // Create Ability System Component for enemies
    EnemyAbilitySystemComponent = CreateDefaultSubobject<UEnemyAbilitySystemComponent>(TEXT("EnemyAbilitySystemComponent"));
    EnemyAbilitySystemComponent->SetIsReplicated(true);

    // Create the Attribute Set for Enemies
    EnemyAttributeSet = CreateDefaultSubobject<UEnemyAttributeSet>(TEXT("EnemyAttributeSet"));

    // Create the UI Health Bar Component
    UIFloatingStatusBarComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("UIFloatingStatusBarComponent"));
    UIFloatingStatusBarComponent->SetupAttachment(RootComponent);
    UIFloatingStatusBarComponent->SetRelativeLocation(FVector(0, 0, 120));
    UIFloatingStatusBarComponent->SetWidgetSpace(EWidgetSpace::Screen);
    UIFloatingStatusBarComponent->SetDrawSize(FVector2D(500, 500));

    // Load the health bar UI class
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
        InitializeAttributes();  // Initialize attributes
        AddStartupEffects();
        UpdateMovementSpeed();// Apply startup effects (if any)
    }

    if (EnemyAbilitySystemComponent)
    {
        EnemyAbilitySystemComponent->RegisterGameplayTagEvent(
            FGameplayTag::RequestGameplayTag(FName("GameplayCue.Debuff.Stun")),
            EGameplayTagEventType::NewOrRemoved
        ).AddUObject(this, &ABP_Enemy::OnTagChanged);
    }
    // Setup health and max health values from the applied GameplayEffect
    HealthChangedDelegateHandle = EnemyAbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(
        EnemyAttributeSet->GetHealthAttribute()).AddUObject(this, &ABP_Enemy::HealthChanged);

    MaxHealthChangedDelegateHandle = EnemyAbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(
        EnemyAttributeSet->GetMaxHealthAttribute()).AddUObject(this, &ABP_Enemy::OnMaxHealthChangedDelegate);

    SetHealth(GetMaxHealth());
    UE_LOG(LogTemp, Warning, TEXT("Health initialized to: %f / %f"), GetHealth(), GetMaxHealth());

    APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0);
    if (PC && PC->IsLocalPlayerController())
    {
        if (UIFloatingStatusBarClass)
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

// Add this method to define the GetEnemyAttributeSet function
UEnemyAttributeSet* ABP_Enemy::GetEnemyAttributeSet() const
{
    return EnemyAttributeSet;
}

// Get Ability System Component
UAbilitySystemComponent* ABP_Enemy::GetAbilitySystemComponent() const
{
    return EnemyAbilitySystemComponent;
}

// Initialize attributes based on the GameplayEffect
void ABP_Enemy::InitializeAttributes()
{
    if (EnemyAbilitySystemComponent && DefaultAttributesForEnemies)
    {
        FGameplayEffectContextHandle EffectContext = EnemyAbilitySystemComponent->MakeEffectContext();
        EffectContext.AddSourceObject(this);
        FGameplayEffectSpecHandle SpecHandle = EnemyAbilitySystemComponent->MakeOutgoingSpec(DefaultAttributesForEnemies, GetCharacterLevel(), EffectContext);

        if (SpecHandle.IsValid())
        {
            FActiveGameplayEffectHandle ActiveGEHandle = EnemyAbilitySystemComponent->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
            SetHealth(GetMaxHealth());
            UpdateMovementSpeed();// Set initial health to max
            UE_LOG(LogTemp, Warning, TEXT("Attributes Initialized: Health set to %f"), GetHealth());
        }
        else
        {
            UE_LOG(LogTemp, Error, TEXT("Failed to apply DefaultAttributes to Enemy: %s"), *GetName());
        }
    }
}

void ABP_Enemy::HealthChanged(const FOnAttributeChangeData& Data)
{
    if (IsPendingKillPending())
    {
        // Prevent any further logic if this actor is pending destruction
        return;
    }

    float NewHealth = Data.NewValue;

    UE_LOG(LogTemp, Warning, TEXT("HealthChanged: New Health = %f for %s"), NewHealth, *GetName());

    UpdateHealthBar();  // Update UI

    if (NewHealth <= 0.0f) // Check if the enemy is dead
    {
        Die();  // Trigger death sequence
    }
}



void ABP_Enemy::OnMaxHealthChangedDelegate(const FOnAttributeChangeData& Data)
{
    float OldMaxHealth = Data.OldValue;
    float NewMaxHealth = Data.NewValue;

    UE_LOG(LogTemp, Warning, TEXT("Max Health changed for %s: Old Max Health = %f, New Max Health = %f"), *GetName(), OldMaxHealth, NewMaxHealth);

    // Adjust the current health proportionally if the max health changes
    float CurrentHealth = GetHealth();
    if (OldMaxHealth > 0.0f)
    {
        float HealthRatio = CurrentHealth / OldMaxHealth;
        float NewHealth = FMath::Clamp(NewMaxHealth * HealthRatio, 0.0f, NewMaxHealth);
        SetHealth(NewHealth);  // Update the current health to be proportional to the new max health
    }
    else
    {
        SetHealth(NewMaxHealth);  // If old max health was zero, just set health to new max health
    }

    UpdateHealthBar();  // Ensure the health bar reflects the changes
}


// Health bar update method
void ABP_Enemy::UpdateHealthBar()
{
    if (IsPendingKillPending())
    {
        // Don't update the health bar if this enemy is pending kill
        return;
    }

    if (UIFloatingStatusBar)
    {
        float Health = GetHealth();
        float MaxHealth = GetMaxHealth();

        if (MaxHealth > 0)
        {
            float HealthPercentage = FMath::Clamp(Health / MaxHealth, 0.0f, 1.0f);
            UIFloatingStatusBar->SetHealthPercentage(HealthPercentage);
            UE_LOG(LogTemp, Warning, TEXT("HealthBar updated to %f for: %s"), HealthPercentage, *GetName());
        }
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("UIFloatingStatusBar is null in UpdateHealthBar for: %s"), *GetName());
    }
}


// Handle enemy death
void ABP_Enemy::Die()
{
    if (!bIsDead)
    {
        bIsDead = true;
        UE_LOG(LogTemp, Warning, TEXT("Enemy is dead: %s"), *GetName());

        // Perform cleanup actions
        FinishDying();
    }
}

// Cleanup after death
void ABP_Enemy::FinishDying()
{
    // Immediately stop interactions by disabling the collision
    SetActorEnableCollision(false);

    // Optional: Disable AI logic or movement to avoid further updates
    // Disable AI movement component if you are using AI
    if (GetController())
    {
        GetController()->Destroy();
    }

    // Set short lifespan before destruction (like before)
    SetLifeSpan(0.1f);  // This keeps it around just a bit before it is destroyed
}


// Update enemy rotation
void ABP_Enemy::UpdateTargetRotation()
{
    FVector Velocity = GetVelocity();
    if (!Velocity.IsNearlyZero())
    {
        FVector Direction = Velocity.GetSafeNormal2D();
        TargetRotation = Direction.Rotation();
    }
}

// Definition for GetHealth
float ABP_Enemy::GetHealth() const
{
    if (EnemyAttributeSet)
    {
        return EnemyAttributeSet->GetHealth();
    }
    return 0.0f;
}

// Definition for GetMaxHealth
float ABP_Enemy::GetMaxHealth() const
{
    if (EnemyAttributeSet)
    {
        return EnemyAttributeSet->GetMaxHealth();
    }
    return 0.0f;
}

// Tick function to update rotation
void ABP_Enemy::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
    UpdateTargetRotation();

    FRotator NewRotation = FMath::RInterpTo(GetActorRotation(), TargetRotation, DeltaTime, RotationSpeed);
    SetActorRotation(NewRotation);
}

// Apply startup effects
void ABP_Enemy::AddStartupEffects()
{
    if (GetLocalRole() == ROLE_Authority && EnemyAbilitySystemComponent)
    {
        if (bIsFastEnemy && FastMovementSpeedEffect)
        {
            // Apply the fast movement speed effect
            FGameplayEffectSpecHandle SpeedSpecHandle = EnemyAbilitySystemComponent->MakeOutgoingSpec(FastMovementSpeedEffect, 1, EnemyAbilitySystemComponent->MakeEffectContext());
            EnemyAbilitySystemComponent->ApplyGameplayEffectSpecToSelf(*SpeedSpecHandle.Data.Get());
        }
        else if (bIsSlowEnemy && SlowMovementSpeedEffect)
        {
            // Apply the slow movement speed effect
            FGameplayEffectSpecHandle SpeedSpecHandle = EnemyAbilitySystemComponent->MakeOutgoingSpec(SlowMovementSpeedEffect, 1, EnemyAbilitySystemComponent->MakeEffectContext());
            EnemyAbilitySystemComponent->ApplyGameplayEffectSpecToSelf(*SpeedSpecHandle.Data.Get());
        }
    }
}


// Handle stun tag change (for future use)
void ABP_Enemy::StunTagChanged(const FGameplayTag CallbackTag, int32 NewCount)
{
    // Handle stun logic (if needed in future)
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
void ABP_Enemy::Attack()
{
    if (EnemyAbilitySystemComponent->HasMatchingGameplayTag(FGameplayTag::RequestGameplayTag(FName("GameplayCue.Debuff.Stun"))))
    {
        UE_LOG(LogTemp, Warning, TEXT("%s is stunned and cannot attack!"), *GetName());
        return; // Stop the attack if stunned
    }

    // Proceed with attack logic
    UE_LOG(LogTemp, Warning, TEXT("%s is attacking!"), *GetName());
    // Attack implementation here
}

void ABP_Enemy::OnTagChanged(const FGameplayTag CallbackTag, int32 NewCount)
{
    if (CallbackTag == FGameplayTag::RequestGameplayTag(FName("GameplayCue.Debuff.Stun")))
    {
        bool bCanAttack;
        if (NewCount > 0) // Stun tag added
        {
            // Disable movement
            if (GetCharacterMovement())
            {
                GetCharacterMovement()->DisableMovement();
                UE_LOG(LogTemp, Warning, TEXT("%s is stunned. Movement disabled."), *GetName());
            }

            // Disable attacks (optional logic)
            bCanAttack = false; // Add a variable to manage attack state
            UE_LOG(LogTemp, Warning, TEXT("%s is stunned. Attacks disabled."), *GetName());
        }
        else // Stun tag removed
        {
            // Re-enable movement
            if (GetCharacterMovement())
            {
                GetCharacterMovement()->SetMovementMode(MOVE_Walking);
                UpdateMovementSpeed();
                UE_LOG(LogTemp, Warning, TEXT("%s is no longer stunned. Movement restored."), *GetName());
            }

            // Re-enable attacks
            bCanAttack = true;
            UE_LOG(LogTemp, Warning, TEXT("%s is no longer stunned. Attacks enabled."), *GetName());
        }
    }
}

void ABP_Enemy::UpdateMovementSpeed()
{
    if (EnemyAttributeSet && GetCharacterMovement())
    {
        float NewSpeed = EnemyAttributeSet->GetMovementSpeed();
        GetCharacterMovement()->MaxWalkSpeed = NewSpeed;
        UE_LOG(LogTemp, Warning, TEXT("Enemy movement speed set to: %f"), NewSpeed);

        if (!EnemyAbilitySystemComponent->HasMatchingGameplayTag(FGameplayTag::RequestGameplayTag(FName("GameplayCue.Debuff.Stun"))))
        {
            GetCharacterMovement()->MaxWalkSpeed = NewSpeed;
            UE_LOG(LogTemp, Warning, TEXT("Enemy movement speed set to: %f"), NewSpeed);
        }
        else
        {
            GetCharacterMovement()->MaxWalkSpeed = 0.0f; // Ensure speed stays 0 when stunned
        }
    }
}
