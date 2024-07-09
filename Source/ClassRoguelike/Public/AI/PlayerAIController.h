#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "PlayerAIController.generated.h"

/**
 *
 */
UCLASS()
class CLASSROGUELIKE_API APlayerAIController : public AAIController
{
	GENERATED_BODY()

public:

	APlayerAIController();

protected:

	virtual void BeginPlay() override;

	virtual void OnPossess(APawn* InPawn) override;

private:

	/** Blackboard component for the AI */
	UPROPERTY(Transient)
	UBlackboardComponent* BlackboardComponent;

	/** Behavior Tree component for the AI */
	UPROPERTY(Transient)
	UBehaviorTreeComponent* BehaviorTreeComponent;

	/** Blackboard asset */
	UPROPERTY(EditDefaultsOnly, Category = "AI")
	UBlackboardData* BlackboardData;

	/** Behavior Tree asset */
	UPROPERTY(EditDefaultsOnly, Category = "AI")
	UBehaviorTree* BehaviorTree;
};
