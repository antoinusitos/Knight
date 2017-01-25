// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Character.h"
#include "Data.h"
#include "KnightPlayer.generated.h"

class AKnightWeapon;

UENUM(BlueprintType)		//"BlueprintType" is essential to include
enum class EPlayerState : uint8
{
	PS_Idle 	UMETA(DisplayName = "Idle"),
	PS_Walk 	UMETA(DisplayName = "Walk"),
	PS_Run		UMETA(DisplayName = "Run"),
	PS_Attack	UMETA(DisplayName = "Attack"),
	PS_Heal		UMETA(DisplayName = "Heal"),
	PS_Dead		UMETA(DisplayName = "Dead"),
};

UCLASS()
class KNIGHT_API AKnightPlayer : public ACharacter
{
	GENERATED_BODY()

public:
	/** Camera boom positioning the camera behind the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* cameraBoom;

	/** Follow camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* followCamera;

	/** Base turn rate, in deg/sec. Other scaling may affect final turn rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
	float _baseTurnRate;

	/** Base look up/down rate, in deg/sec. Other scaling may affect final rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
	float _baseLookUpRate;

private:

	/** Called for forwards/backward input */
	void MoveForward(float Value);

	/** Called for side to side input */
	void MoveRight(float Value);

	/**
	* Called via input to turn at a given rate.
	* @param Rate	This is a normalized rate, i.e. 1.0 means 100% of desired turn rate
	*/
	void TurnAtRate(float Rate);

	/**
	* Called via input to turn look up/down at a given rate.
	* @param Rate	This is a normalized rate, i.e. 1.0 means 100% of desired turn rate
	*/
	void LookUpAtRate(float Rate);

	bool CanAttack() const;
	void Attack();

	bool CanHeal() const;
	void Heal();

	bool CanMove() const;

	// APawn interface
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	// End of APawn interface

	void Tick(float DeltaTime) override;

	// Initialize variables
	void Init();

	void Run();
	void StopRun();

	void Die();

	UPROPERTY(EditAnywhere, Category = "Player Movement")
	float _staminaSpeedLoose;
	UPROPERTY(EditAnywhere, Category = "Player Movement")
	float _staminaSpeedGain;

	float _lastStaminaLost;
	float _timeToGainStamina;

	UPROPERTY(EditAnywhere, Category = "Player Attack")
	float _staminaAttackLoose;

	EPlayerState _currentPlayerState;

	bool _isMovingForward;
	bool _isMovingRight;
	bool _isRunning;

	int _totalWeight;
	int _maxWeight;

	void GetTotalWeight();

	void LoadObjects();

	UPROPERTY(VisibleAnywhere, Category = "Player Attack")
	AKnightWeapon* _theWeapon;

	UPROPERTY(VisibleAnywhere, Category = "Player Inventory")
	TArray<FWeaponStruct> _weapons;

	UPROPERTY(VisibleAnywhere, Category = "Player Inventory")
	TArray<FClothesStruct> _clothes;

	UPROPERTY(VisibleAnywhere, Category = "Player Inventory")
	TArray<FConsumableStruct> _consumables;

	UPROPERTY(VisibleAnywhere, Category = "Player Inventory")
	TArray<FItemStruct> _others;

	UDataTable* _weaponsTable;
	UDataTable* _clothesTable;
	UDataTable* _consumableTable;

	int32 HaveWeapon(int32 id);
	int32 HaveConsumable(int32 id);
	int32 HaveClothes(int32 id);


// Make these variables protected to tweek them
protected:

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Player Movement")
	float _walkingSpeed;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Player Movement")
	float _runningSpeed;

	// The total life of the player
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Player Infos")
	int _maxLife;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Player Infos")
	int _currentLife;

	// The total stamina of the player
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Player Infos")
	float _maxStamina;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Player Infos")
	float _currentStamina;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player Attack")
	FName _theWeaponToLoad;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player Inventory")
	TArray<int32> _weaponsToLoad;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player Inventory")
	TArray<int32> _clothesToLoad;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player Inventory")
	TArray<int32> _consumablesToLoad;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player Inventory")
	TArray<int32> _othersToLoad;

public:
	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return cameraBoom; }
	/** Returns FollowCamera subobject **/
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return followCamera; }

	AKnightPlayer();

	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PLAYER DEBUG")
	bool ACTIVE_DEBUG;

	UFUNCTION(BlueprintCallable, Category = "Player Infos")
	int GetMaxLife() const;
	UFUNCTION(BlueprintCallable, Category = "Player Infos")
	int GetcurrentLife() const;

	UFUNCTION(BlueprintCallable, Category = "Player Infos")
	float GetMaxStamina() const;
	UFUNCTION(BlueprintCallable, Category = "Player Infos")
	float GetcurrentStamina() const;

	UFUNCTION(BlueprintCallable, Category = "Player Infos")
	EPlayerState GetCurrentPlayerState() const;
	UFUNCTION(BlueprintCallable, Category = "Player Infos")
	void SetCurrentPlayerState(EPlayerState NewPlayerState);

	void PlayerTakeDamage(int amount);
};
