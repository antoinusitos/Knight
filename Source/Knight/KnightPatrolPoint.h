// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "KnightPatrolPoint.generated.h"

UCLASS()
class KNIGHT_API AKnightPatrolPoint : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AKnightPatrolPoint();

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	// Called every frame
	virtual void Tick( float DeltaSeconds ) override;

	AKnightPatrolPoint* GetNextPoint();

	float GetTimeToWait();

protected:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Patrol Point")
	AKnightPatrolPoint* nextPoint;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Patrol Point")
	float timeToWait;
};
