// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Character.h"
#include "Data.h"
#include "KnightPlayer.generated.h"

class AKnightWeapon;
class AKnightClothes;
class AKnightConsumable;
class AKnightSeller;

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

	void Turn(float Value);

	void LookUp(float Value);

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

	void Action();

	void Cancel();

	// the speed of the loose of stamina
	UPROPERTY(EditAnywhere, Category = "Player Movement")
	float _staminaSpeedLoose;
	// the speed of the gain of stamina
	UPROPERTY(EditAnywhere, Category = "Player Movement")
	float _staminaSpeedGain;

	// current time waited for the stamina to refill
	float _lastStaminaLost;
	// the time before the refill of stamina
	float _timeToGainStamina;

	// the amount of stamina lost when attacking
	UPROPERTY(EditAnywhere, Category = "Player Attack")
	float _staminaAttackLoose;

	EPlayerState _currentPlayerState;

	bool _isMovingForward;
	bool _isMovingRight;
	bool _isRunning;

	int _totalWeight;
	int _maxWeight;

	void GetTotalWeight();

	// load the objects that must be equipped
	void LoadObjects();
	void EquipWeapons();
	void EquipClothes();
	void EquipConsumable();

	// EQUIPPED

	// Weapons
	UPROPERTY(VisibleAnywhere, Category = "Player Inventory")
	AKnightWeapon* _firstWeapon;

	UPROPERTY(VisibleAnywhere, Category = "Player Inventory")
	AKnightWeapon* _secondWeapon;

	// clothes
	UPROPERTY(VisibleAnywhere, Category = "Player Inventory")
	AKnightClothes* _helmet;

	UPROPERTY(VisibleAnywhere, Category = "Player Inventory")
	AKnightClothes* _torso;

	UPROPERTY(VisibleAnywhere, Category = "Player Inventory")
	AKnightClothes* _greaves;

	UPROPERTY(VisibleAnywhere, Category = "Player Inventory")
	AKnightClothes* _gloveLeft;

	UPROPERTY(VisibleAnywhere, Category = "Player Inventory")
	AKnightClothes* _gloveRight;

	UPROPERTY(VisibleAnywhere, Category = "Player Inventory")
	AKnightClothes* _shoeLeft;

	UPROPERTY(VisibleAnywhere, Category = "Player Inventory")
	AKnightClothes* _shoeRight;

	UPROPERTY(VisibleAnywhere, Category = "Player Inventory")
	AKnightClothes* _firstRing;

	UPROPERTY(VisibleAnywhere, Category = "Player Inventory")
	AKnightClothes* _secondRing;

	// quick Slot
	UPROPERTY(VisibleAnywhere, Category = "Player Inventory")
	AKnightConsumable* _firstSlot;

	UPROPERTY(VisibleAnywhere, Category = "Player Inventory")
	AKnightConsumable* _secondSlot;

	// INVENTORY

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

	int _gold;

	AKnightSeller* _sellerToOpen;
	bool _shopOpen;

	UFUNCTION()
	void OnPlayerDetectionOverlapBegin(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult);
	UFUNCTION()
	void OnPlayerDetectionOverlapEnd(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);


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

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player Detection")
	USphereComponent* _playerDetection;

	UFUNCTION(BlueprintImplementableEvent, Category = "Player Shop")
	void OpenShop(AKnightSeller* shopToOpen);

	UFUNCTION(BlueprintImplementableEvent, Category = "Player Shop")
	void HideShop();

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

	// Current Equiped
	UFUNCTION(BlueprintCallable, Category = "Player Infos")
	AKnightWeapon* GetWeaponOne();
	UFUNCTION(BlueprintCallable, Category = "Player Infos")
	AKnightWeapon* GetWeaponTwo();

	UFUNCTION(BlueprintCallable, Category = "Player Infos")
	AKnightClothes* GetHelmet();
	UFUNCTION(BlueprintCallable, Category = "Player Infos")
	AKnightClothes* GetTorso();
	UFUNCTION(BlueprintCallable, Category = "Player Infos")
	AKnightClothes* GetGreaves();
	UFUNCTION(BlueprintCallable, Category = "Player Infos")
	AKnightClothes* GetGloveRight();
	UFUNCTION(BlueprintCallable, Category = "Player Infos")
	AKnightClothes* GetGloveLeft();
	UFUNCTION(BlueprintCallable, Category = "Player Infos")
	AKnightClothes* GetShoeRight();
	UFUNCTION(BlueprintCallable, Category = "Player Infos")
	AKnightClothes* GetShoeLeft();


	// Inventory
	UFUNCTION(BlueprintCallable, Category = "Player Infos")
	TArray<FWeaponStruct> GetWeapons();
	UFUNCTION(BlueprintCallable, Category = "Player Infos")
	TArray<FClothesStruct> GetClothes();
	UFUNCTION(BlueprintCallable, Category = "Player Infos")
	TArray<FConsumableStruct> GetConsummable();
	UFUNCTION(BlueprintCallable, Category = "Player Infos")
	TArray<FItemStruct> GetItem();
};
