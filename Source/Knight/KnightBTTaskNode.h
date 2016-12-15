// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "BehaviorTree/BTTaskNode.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "KnightBTTaskNode.generated.h"

class AKnightPatrolPoint;
class AKnightAIController;

/**
 * 
 */
UCLASS()
class KNIGHT_API UKnightBTTaskNode : public UBTTaskNode
{
	GENERATED_BODY()
	
public:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	
};
