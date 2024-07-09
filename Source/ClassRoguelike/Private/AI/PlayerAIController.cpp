#include "AI/PlayerAIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "BehaviorTree/BehaviorTree.h"
#include "GameFramework/Character.h"

APlayerAIController::APlayerAIController()
{
	bWantsPlayerState = true;

	// Initialize the blackboard and behavior tree components
	BlackboardComponent = CreateDefaultSubobject<UBlackboardComponent>(TEXT("BlackboardComponent"));
	BehaviorTreeComponent = CreateDefaultSubobject<UBehaviorTreeComponent>(TEXT("BehaviorTreeComponent"));
}

void APlayerAIController::BeginPlay()
{
	Super::BeginPlay();
}

void APlayerAIController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	if (BlackboardData && BehaviorTree)
	{
		if (BlackboardComponent)
		{
			BlackboardComponent->InitializeBlackboard(*BlackboardData);
		}

		if (BehaviorTreeComponent)
		{
			BehaviorTreeComponent->StartTree(*BehaviorTree);
		}
	}
}
