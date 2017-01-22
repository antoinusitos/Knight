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

	// set up a notification for when we detect the player
	_playerDetection->OnComponentBeginOverlap.AddDynamic(this, &AKnightAI::OnPlayerDetectedOverlapBegin);
	_playerDetection->OnComponentEndOverlap.AddDynamic(this, &AKnightAI::OnPlayerDetectedOverlapEnd);

	// set up a notification for when we must face the player and attack him
	_playerRangeAttack->OnComponentBeginOverlap.AddDynamic(this, &AKnightAI::OnPlayerRangeAttackOverlapBegin);
	_playerRangeAttack->OnComponentEndOverlap.AddDynamic(this, &AKnightAI::OnPlayerRangeAttackOverlapEnd);

	Init();
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
	_distToIdle = 100.0f;

	// idle
	_timeToWait = 5.0f;
	_currentTimeToWait = 0.0f;

	_target = nullptr;
	_destination = FVector::ZeroVector;

	// attack
	_attackRate = 1.0f;
	_currentAttackWait = 0.0f;
	_canAttack = true;
	_attackAmount = 30;
	_isAttacking = false;
	isInRangeToAttack = false;

	_mustPatrol = false;
}

// Called when the game starts or when spawned
void AKnightAI::BeginPlay()
{
	Super::BeginPlay();

	if (GetController())
	{
		_controller = Cast<AKnightAIController>(GetController());
	}
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
			if (isInRangeToAttack)
			{
				_currentState = EAIState::AIS_Attack;
			}
		}
	}

	switch (_currentState)
	{
	case EAIState::AIS_Dead:
		break;

	case EAIState::AIS_Patrol:
		if (_mustPatrol)
		{
			_currentTimeToWait = 0.0f;
			FindPatrolPoint();
			if (_controller != nullptr)
			{
				MoveToDestinationPoint(_destination);
			}
		}
		break;
	case EAIState::AIS_Idle:
		_currentTimeToWait += DeltaTime;
		if (_currentTimeToWait >= _timeToWait)
		{
			_currentTimeToWait = 0.0f;
			_currentState = EAIState::AIS_Patrol;
		}
		break;
	case EAIState::AIS_Follow:
		if (_controller != nullptr && _target != nullptr)
		{
			_destination = _target->GetActorLocation();
			MoveToDestinationPoint(_destination);
		}
		break;
	case EAIState::AIS_Attack:
		_isAttacking = true;
		_controller->MoveToLocation(GetActorLocation());
		break;
	}
}

void AKnightAI::FindPatrolPoint()
{
	if (_nextPoint == nullptr)
	{
		for (TActorIterator<AKnightPatrolPoint> ActorItr(GetWorld()); ActorItr; ++ActorItr)
		{
			_nextPoint = *ActorItr;
			break;
		}
	}

	if(_nextPoint->GetNextPoint())
		_destination = _nextPoint->GetNextPoint()->GetActorLocation();
}

void AKnightAI::MoveToDestinationPoint(const FVector& theDestination)
{
	_controller->MoveToLocation(theDestination);
	float dist = FVector::Dist(theDestination, GetActorLocation());
	if (dist <= _distToIdle)
	{
		_currentState = EAIState::AIS_Idle;
		if(_nextPoint)
			_nextPoint = _nextPoint->GetNextPoint();
		else
		{
			for (TActorIterator<AKnightPatrolPoint> ActorItr(GetWorld()); ActorItr; ++ActorItr)
			{
				_nextPoint = *ActorItr;
				break;
			}
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

void AKnightAI::OnPlayerDetectedOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	AKnightPlayer* player = Cast<AKnightPlayer>(OtherActor);
	if (player == nullptr) return;

	if(_currentState != EAIState::AIS_Dead && _currentState != EAIState::AIS_Follow)
	{
		_currentState = EAIState::AIS_Follow;
		_target = Cast<AKnightPlayer>(OtherActor);
	}
}

void AKnightAI::OnPlayerDetectedOverlapEnd(UPrimitiveComponent * OverlappedComp, AActor * OtherActor, UPrimitiveComponent * OtherComp, int32 OtherBodyIndex)
{
	AKnightPlayer* player = Cast<AKnightPlayer>(OtherActor);
	if (player == nullptr) return;

	if (_currentState != EAIState::AIS_Dead && _currentState == EAIState::AIS_Follow)
	{
		_currentState = EAIState::AIS_Patrol;
		_target = nullptr;
	}
}

void AKnightAI::OnPlayerRangeAttackOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	AKnightPlayer* player = Cast<AKnightPlayer>(OtherActor);
	if (player == nullptr) return;

	isInRangeToAttack = true;

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
	AKnightPlayer* player = Cast<AKnightPlayer>(OtherActor);
	if (player == nullptr) return;

	isInRangeToAttack = false;

	if (_currentState != EAIState::AIS_Dead && _currentState == EAIState::AIS_Attack && !_isAttacking)
	{
		UE_LOG(LogTemp, Warning, TEXT("muse follow"));
		_currentState = EAIState::AIS_Follow;
	}
}

void AKnightAI::TestPlayerIsAround()
{
	_isAttacking = false;

	_currentState = EAIState::AIS_Idle;

	TArray<AActor*> overlappingActors;
	_playerDetection->GetOverlappingActors(overlappingActors, TSubclassOf<AKnightPlayer>());
	TArray<AActor*> overlappingActorsToAttack;
	_playerRangeAttack->GetOverlappingActors(overlappingActorsToAttack, TSubclassOf<AKnightPlayer>());

	for (int i = 0; i < overlappingActorsToAttack.Num(); i++)
	{
		auto player = Cast<AKnightPlayer>(overlappingActorsToAttack[i]);
		if (player != nullptr)
		{
			_currentState = EAIState::AIS_Attack;
			return;
		}
	}

	for (int i = 0; i < overlappingActors.Num(); i++)
	{
		auto player = Cast<AKnightPlayer>(overlappingActors[i]);
		if (player != nullptr)
		{
			_currentState = EAIState::AIS_Follow;
			return;
		}
	}

	_currentState = EAIState::AIS_Patrol;
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