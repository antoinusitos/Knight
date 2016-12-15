// Fill out your copyright notice in the Description page of Project Settings.

#include "Knight.h"
#include "KnightAIController.h"
#include "KnightPatrolPoint.h"
#include "KnightAI.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "BehaviorTree/BehaviorTree.h"

AKnightAIController::AKnightAIController()
{
	//Initialize BehaviorTreeComponent, BlackboardComponent and the corresponding key

	_behaviorComp = CreateDefaultSubobject<UBehaviorTreeComponent>(TEXT("BehaviorComp"));

	_blackboardComp = CreateDefaultSubobject<UBlackboardComponent>(TEXT("BlackboardComp"));

	_locationToGoKey = "LocationToGo";
}

void AKnightAIController::Possess(APawn* thePawn)
{
	Super::Possess(thePawn);

	//Get the possessed Character and check if it's my own AI Character
	AKnightAI* AIChar = Cast<AKnightAI>(thePawn);

	if (AIChar)
	{
		//If the blackboard is valid initialize the blackboard for the corresponding behavior tree
		if (AIChar->GetBehaviorTree()->BlackboardAsset)
		{
			_blackboardComp->InitializeBlackboard(*(AIChar->GetBehaviorTree()->BlackboardAsset));
		}

		/*Populate the array of available bot target points
		The following function needs a TArray of AActors, that's why I declared the
		BotTargetPoints as such. When I will need to get an exact point and compare it,
		I will cast it to the corresponding class (ABotTargetPoint)*/
		UGameplayStatics::GetAllActorsOfClass(GetWorld(), AKnightPatrolPoint::StaticClass(), _botTargetPoints);

		//Start the behavior tree which corresponds to the specific character
		_behaviorComp->StartTree(*AIChar->GetBehaviorTree());
	}
}