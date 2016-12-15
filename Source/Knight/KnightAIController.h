// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "AIController.h"
#include "KnightAIController.generated.h"

class AKnightPatrolPoint;

/**
 * 
 */
UCLASS()
class KNIGHT_API AKnightAIController : public AAIController
{
	GENERATED_BODY()
	
public:
	AKnightAIController();

	FORCEINLINE UBlackboardComponent* GetBlackboardComp() const { return _blackboardComp; }

	FORCEINLINE TArray<AActor*> GetAvailableTargetPoints() { return _botTargetPoints; }

private:

	/*Behavior tree comp ref*/
	class UBehaviorTreeComponent* _behaviorComp;

	/*----------Blackboard----------*/

	/*Blackboard comp ref*/
	class UBlackboardComponent* _blackboardComp;

	/*Blackboard keys*/
	UPROPERTY(EditDefaultsOnly, Category = "AI")
	FName _locationToGoKey;

	/*----------Blackboard----------*/

	/*I use the AActor class as the Template class so I can use the GetAllActorsOfClass function.
	However, in this collection I'm going to store all the Available ABotTargetPoint instances.*/
	TArray<AActor*> _botTargetPoints;

	/*Posses is executed when the character we want to control is spawned.
	Inside this function, we initialize the blackboard and start the behavior tree*/
	virtual void Possess(APawn* Pawn) override;
	
};
