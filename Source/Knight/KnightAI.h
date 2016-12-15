// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Character.h"
#include "KnightAI.generated.h"

UENUM(BlueprintType)		//"BlueprintType" is essential to include
enum class EAIState : uint8
{
	AIS_Idle 	UMETA(DisplayName = "Idle"),
	AIS_Patrol 	UMETA(DisplayName = "Patrol"),
	AIS_Attack	UMETA(DisplayName = "Attack"),
	AIS_Follow	UMETA(DisplayName = "Follow"),
	AIS_Dead	UMETA(DisplayName = "Dead"),
};

class AKnightPatrolPoint;

UCLASS()
class KNIGHT_API AKnightAI : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AKnightAI();

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	// Called every frame
	virtual void Tick( float DeltaSeconds ) override;

	void ReceiveDamage(int damage);

private:
	bool CanAttack() const;
	void Attack();

	bool CanMove() const;

	// Initialize variables
	void Init();

	EAIState _currentState;

	UFUNCTION()
	void OnPlayerDetectedOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnPlayerRangeAttackOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION()
	void OnPlayerRangeAttackOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI Movement")
	float _walkingSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI Movement")
	float _runningSpeed;

	// The total life of the AI
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AI Infos")
	int _maxLife;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI Infos")
	int _currentLife;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI Detection")
	USphereComponent* _playerDetection;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI Range Attack")
	USphereComponent* _playerRangeAttack;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI Movement")
	AKnightPatrolPoint* nextPoint;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
	class UBehaviorTree* BehaviorTree;

public:
	UFUNCTION(BlueprintCallable, Category = "AI Infos")
	int GetMaxLife() const;
	UFUNCTION(BlueprintCallable, Category = "AI Infos")
	int GetcurrentLife() const;
	
	UFUNCTION(BlueprintCallable, Category = "AI Infos")
	EAIState GetCurrentAIState() const;
	UFUNCTION(BlueprintCallable, Category = "AI Infos")
	void SetCurrentAIState(EAIState NewAIState);

	FORCEINLINE class UBehaviorTree* GetBehaviorTree() const { return BehaviorTree; }
};
