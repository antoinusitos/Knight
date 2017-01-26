// Fill out your copyright notice in the Description page of Project Settings.

#include "Knight.h"
#include "KnightObject.h"


// Sets default values
AKnightObject::AKnightObject()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AKnightObject::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AKnightObject::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );

}