// Fill out your copyright notice in the Description page of Project Settings.

#include "Knight.h"
#include "KnightPatrolPoint.h"


// Sets default values
AKnightPatrolPoint::AKnightPatrolPoint()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	timeToWait = 1.0f;
}

// Called when the game starts or when spawned
void AKnightPatrolPoint::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AKnightPatrolPoint::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );

}

AKnightPatrolPoint* AKnightPatrolPoint::GetNextPoint()
{
	return nextPoint;
}

float AKnightPatrolPoint::GetTimeToWait()
{
	return timeToWait;
}