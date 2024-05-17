// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CharacterBase.h"
#include "Player/MainPlayerState.h"
#include "MainPlayerCharacter.generated.h"

/**
 * 
 */
UCLASS()
class CLASSROGUELIKE_API AMainPlayerCharacter : public ACharacterBase 
{
	GENERATED_BODY()

public:

	AMainPlayerCharacter(const class FObjectInitializer& ObjectInitializer);

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	virtual void PossessedBy(AController* NewController) override;

	class USpringArmComponent* GetCameraBoom();

	class UCameraComponent* GetFollowCamera();

	UFUNCTION(BlueprintCallable, Category = "ClassRogueLike|Camera")
	float GetStartingCameraBoomArmLength();

	UFUNCTION(BlueprintCallable, Category = "ClassRogueLike|Camera")
	FVector GetStartingCameraBoomLocation();

protected:

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "ClassRogueLike|Camera")
	float BaseTurnRate = 45.0f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "ClassRogueLike|Camera")
	float BaseLookupRate = 45.0f;

	UPROPERTY(BlueprintReadOnly, Category = "ClassRogueLike|Camera")
	float StartingCameraBoomArmLength;

	UPROPERTY(BlueprintReadOnly, Category = "ClassRogueLike|Camera")
	FVector StartingCameraBoomLocation;

	UPROPERTY(BlueprintReadOnly,VisibleAnywhere, Category = "ClassRogueLike|Camera")
	class USpringArmComponent* CameraBoom;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "ClassRogueLike|Camera")
	class UCameraComponent* FollowCamera;


	bool ASCInputBound = false;

	FGameplayTag DeadTag;

	virtual void BeginPlay() override;

	void LookUp(float Value);

	void LookUpRate(float Value);

	void Turn(float Value);

	void TurnRate(float Value);
	
	void MoveForward(float Value);

	void MoveRight(float Value);

	virtual void OnRep_PlayerState() override;

	void InitializeStartingValues(AMainPlayerState* PS);

	void BindASCInput();


	

};
