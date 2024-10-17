#pragma once

#include "CoreMinimal.h"
#include "CharacterBase.h"
#include "Player/MainPlayerState.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputActionValue.h"// Include necessary headers
#include "MotionWarpingComponent.h"
#include "MainPlayerCharacter.generated.h"

UCLASS()
class CLASSROGUELIKE_API AMainPlayerCharacter : public ACharacterBase
{
    GENERATED_BODY()

public:
    AMainPlayerCharacter(const class FObjectInitializer& ObjectInitializer);

    virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

    void HandleInputActionTriggered(const FInputActionInstance& ActionInstance);
    void HandleInputActionCompleted(const FInputActionInstance& ActionInstance);
    virtual void PossessedBy(AController* NewController) override;

    class USpringArmComponent* GetCameraBoom();
    class UCameraComponent* GetFollowCamera();

    UFUNCTION(BlueprintCallable, Category = "ClassRogueLike|Camera")
    float GetStartingCameraBoomArmLength();

    UFUNCTION(BlueprintCallable, Category = "ClassRogueLike|Camera")
    FVector GetStartingCameraBoomLocation();

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
    class UInputMappingContext* MovementMappingContext;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
    class UInputAction* UseAbilityAction;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
    class UInputMappingContext* AbilityMappingContext;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
    class UInputMappingContext* FighterAbilitiesMappingContext;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
    class UInputAction* StartTargetingAction;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
    class UInputAction* ConfirmTargetAction;

    void BindASCInput();
    void OnRep_PlayerState() override;

protected:
    UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "ClassRogueLike|Camera")
    float BaseTurnRate = 45.0f;

    UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "ClassRogueLike|Camera")
    float BaseLookupRate = 45.0f;

    UPROPERTY(BlueprintReadOnly, Category = "ClassRogueLike|Camera")
    float StartingCameraBoomArmLength;

    UPROPERTY(BlueprintReadOnly, Category = "ClassRogueLike|Camera")
    FVector StartingCameraBoomLocation;

    UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "ClassRogueLike|Camera")
    class USpringArmComponent* CameraBoom;

    UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "ClassRogueLike|Camera")
    class UCameraComponent* FollowCamera;

    bool ASCInputBound = false;
    FGameplayTag DeadTag;

    virtual void BeginPlay() override;

    void LookUp(const FInputActionValue& Value);
    void LookUpRate(const FInputActionValue& Value);
    void Turn(const FInputActionValue& Value);
    void TurnRate(const FInputActionValue& Value);
    void MoveForward(const FInputActionValue& Value);
    void MoveRight(const FInputActionValue& Value);
    void InitializeStartingValues(AMainPlayerState* PS);
    void UseAbility(const FInputActionValue& Value);
    void StartTargeting(const FInputActionValue& Value);
    void ConfirmTarget(const FInputActionValue& Value);

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
    class UInputAction* LookUpAction;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
    class UInputAction* LookUpRateAction;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
    class UInputAction* TurnAction;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
    class UInputAction* TurnRateAction;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
    class UInputAction* MoveForwardAction;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
    class UInputAction* MoveRightAction;

    TMap<UInputAction*, FGameplayAbilitySpecHandle> InputToAbilityMap;

    void ActivateAbility(UInputAction* InputAction);

    // New properties for attack detection
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat")
    class UCapsuleComponent* AttackCollision;

    // Function to handle overlap events
    UFUNCTION()
    void OnAttackOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
        UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

    // Function to activate/deactivate attack collision
    void ActivateAttack();
    void DeactivateAttack();

    // Timer handle for deactivating attack
    FTimerHandle AttackTimerHandle;
};
