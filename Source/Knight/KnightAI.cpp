// Fill out your copyright notice in the Description page of Project Settings.

#include "Knight.h"
#include "KnightAI.h"


// Sets default values
AKnightAI::AKnightAI()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	_playerDetection = CreateDefaultSubobject<USphereComponent>(FName("Player Detection"));
	_playerDetection->SetupAttachment(GetCapsuleComponent());
	_playerDetection->SetSphereRadius(500.0f);


	_playerRangeAttack = CreateDefaultSubobject<USphereComponent>(FName("Player Range Attack"));
	_playerRangeAttack->SetupAttachment(GetCapsuleComponent());
	_playerRangeAttack->SetSphereRadius(200.0f);

	Init();
}

// Called when the game starts or when spawned
void AKnightAI::BeginPlay()
{
	Super::BeginPlay();
	
	// set up a notification for when we detect the player
	_playerDetection->OnComponentBeginOverlap.AddDynamic(this, &AKnightAI::OnPlayerDetectedOverlapBegin);

	// set up a notification for when we must face the player and attack him
	_playerRangeAttack->OnComponentBeginOverlap.AddDynamic(this, &AKnightAI::OnPlayerRangeAttackOverlapBegin);
	_playerRangeAttack->OnComponentEndOverlap.AddDynamic(this, &AKnightAI::OnPlayerRangeAttackOverlapEnd);
}

// Called every frame
void AKnightAI::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );

}

void AKnightAI::ReceiveDamage(int damage)
{
	if (_currentState == EAIState::AIS_Dead) return;

	_currentLife -= damage;
	if(_currentLife <= 0)
	{
		// DEAD
		UE_LOG(LogTemp, Warning, TEXT("AI DEAD"));
		_currentState = EAIState::AIS_Dead;
	}
}

bool AKnightAI::CanAttack() const
{
	return false;
}

void AKnightAI::Attack()
{

}

bool AKnightAI::CanMove() const
{
	return false;
}

void AKnightAI::Init()
{
	_currentState = EAIState::AIS_Idle;

	_maxLife = 100;
	_currentLife = _maxLife;

	_walkingSpeed = 300.0f;
	_runningSpeed = 600.0f;
	GetCharacterMovement()->MaxWalkSpeed = _walkingSpeed;
}

void AKnightAI::OnPlayerDetectedOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if(_currentState != EAIState::AIS_Follow)
	{
		_currentState = EAIState::AIS_Follow;
	}


}

void AKnightAI::OnPlayerRangeAttackOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (_currentState != EAIState::AIS_Attack)
	{
		_currentState = EAIState::AIS_Attack;
	}
}

void AKnightAI::OnPlayerRangeAttackOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (_currentState == EAIState::AIS_Attack)
	{
		_currentState = EAIState::AIS_Follow;
	}
}

int AKnightAI::GetMaxLife() const
{
	return _maxLife;
}

int AKnightAI::GetcurrentLife() const
{
	return _currentLife;
}

EAIState AKnightAI::GetCurrentAIState() const
{
	return _currentState;
}

void AKnightAI::SetCurrentAIState(EAIState NewAIState)
{
	_currentState = NewAIState;
}
