// Fill out your copyright notice in the Description page of Project Settings.

#include "Knight.h"
#include "KnightAI.h"
#include "KnightPatrolPoint.h"
#include "KnightAIController.h"
#include "KnightPlayer.h"

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

	_attackSphere = CreateDefaultSubobject<USphereComponent>(FName("Player Attack Sphere"));
	_attackSphere->SetupAttachment(GetCapsuleComponent());
	_attackSphere->SetSphereRadius(50.0f);

	Init();
}

// Called when the game starts or when spawned
void AKnightAI::BeginPlay()
{
	Super::BeginPlay();
	
	// set up a notification for when we detect the player
	_playerDetection->OnComponentBeginOverlap.AddDynamic(this, &AKnightAI::OnPlayerDetectedOverlapBegin);
	_playerDetection->OnComponentEndOverlap.AddDynamic(this, &AKnightAI::OnPlayerDetectedOverlapEnd);

	// set up a notification for when we must face the player and attack him
	_playerRangeAttack->OnComponentBeginOverlap.AddDynamic(this, &AKnightAI::OnPlayerRangeAttackOverlapBegin);
	_playerRangeAttack->OnComponentEndOverlap.AddDynamic(this, &AKnightAI::OnPlayerRangeAttackOverlapEnd);
}

// Called every frame
void AKnightAI::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );

	if (!_canAttack)
	{
		_currentAttackWait += DeltaTime;
		if (_currentAttackWait >= _attackRate)
		{
			_canAttack = true;
			_currentAttackWait = 0.0f;
		}
	}

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
	return _canAttack;
}

void AKnightAI::Attack()
{
	if (_canAttack)
	{
		TArray<AActor*> player;
		_attackSphere->GetOverlappingActors(player, TSubclassOf<ACharacter>());
		for (int i = 0; i < player.Num(); ++i)
		{
			auto aPlayer = Cast<AKnightPlayer>(player[i]);
			if (aPlayer)
			{
				aPlayer->PlayerTakeDamage(_attackAmount);
			}
		}
		_canAttack = false;
	}
}

bool AKnightAI::CanMove() const
{
	return _currentState != EAIState::AIS_Dead && _currentState != EAIState::AIS_Attack;
}

void AKnightAI::Init()
{
	_currentState = EAIState::AIS_Idle;

	// stats
	_maxLife = 100;
	_currentLife = _maxLife;

	// movment
	_walkingSpeed = 300.0f;
	_runningSpeed = 600.0f;
	GetCharacterMovement()->MaxWalkSpeed = _walkingSpeed;

	// attack
	_attackRate = 1.0f;
	_currentAttackWait = 0.0f;
	_canAttack = true;
	_attackAmount = 30;
	_isAttacking = false;
}

void AKnightAI::OnPlayerDetectedOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if(_currentState != EAIState::AIS_Dead && _currentState != EAIState::AIS_Follow)
	{
		_currentState = EAIState::AIS_Follow;
		UE_LOG(LogTemp, Warning, TEXT("back to follow"));
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("back to follow other"));
	}
}

void AKnightAI::OnPlayerDetectedOverlapEnd(UPrimitiveComponent * OverlappedComp, AActor * OtherActor, UPrimitiveComponent * OtherComp, int32 OtherBodyIndex)
{
	if (_currentState != EAIState::AIS_Dead && _currentState == EAIState::AIS_Follow)
	{
		_currentState = EAIState::AIS_Patrol;
		UE_LOG(LogTemp, Warning, TEXT("back to patrol"));
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("back to patrol other"));
	}
}

void AKnightAI::OnPlayerRangeAttackOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (_currentState != EAIState::AIS_Dead && _currentState != EAIState::AIS_Attack)
	{
		if (_canAttack)
		{
			_currentState = EAIState::AIS_Attack;
		}
	}
}

void AKnightAI::OnPlayerRangeAttackOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (_currentState != EAIState::AIS_Dead && _currentState == EAIState::AIS_Attack && !_isAttacking)
	{
		_currentState = EAIState::AIS_Follow;
	}
}

void AKnightAI::TestPlayerIsAround()
{
	_isAttacking = false;

	TArray<AActor*> overlappingActors;
	_playerDetection->GetOverlappingActors(overlappingActors, TSubclassOf<AKnightPlayer>());
	TArray<AActor*> overlappingActorsToAttack;
	_playerRangeAttack->GetOverlappingActors(overlappingActorsToAttack, TSubclassOf<AKnightPlayer>());

	if (overlappingActors.Num() > 0 && overlappingActorsToAttack.Num() == 0)
	{
		_currentState = EAIState::AIS_Follow;
	}
	else if (overlappingActorsToAttack.Num() == 0)
	{
		_currentState = EAIState::AIS_Patrol;
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