// Fill out your copyright notice in the Description page of Project Settings.

#include "Knight.h"
#include "KnightPlayer.h"
#include "KnightWeapon.h"
#include "KnightClothes.h"
#include "KnightConsumable.h"
#include "KnightSeller.h"

// Sets default values
AKnightPlayer::AKnightPlayer()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

	// set our turn rates for input
	_baseTurnRate = 45.f;
	_baseLookUpRate = 45.f;

	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true; // Character moves in the direction of input...	
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 540.0f, 0.0f); // ...at this rotation rate
	GetCharacterMovement()->JumpZVelocity = 600.f;
	GetCharacterMovement()->AirControl = 0.2f;

	// Create a camera boom (pulls in towards the player if there is a collision)
	cameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	cameraBoom->SetupAttachment(RootComponent);
	cameraBoom->TargetArmLength = 300.0f; // The camera follows at this distance behind the character	
	cameraBoom->bUsePawnControlRotation = true; // Rotate the arm based on the controller

	// Create a follow camera
	followCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	followCamera->SetupAttachment(cameraBoom, USpringArmComponent::SocketName); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	followCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

	_playerDetection = CreateDefaultSubobject<USphereComponent>(TEXT("player detection"));
	_playerDetection->SetupAttachment(RootComponent);
	_playerDetection->OnComponentBeginOverlap.AddDynamic(this, &AKnightPlayer::OnPlayerDetectionOverlapBegin);
	_playerDetection->OnComponentEndOverlap.AddDynamic(this, &AKnightPlayer::OnPlayerDetectionOverlapEnd);

	ConstructorHelpers::FObjectFinder<UDataTable> weaponTable_BP(TEXT("DataTable'/Game/Data/Tables/WeaponDataTable.WeaponDataTable'"));
	_weaponsTable = weaponTable_BP.Object;
	ConstructorHelpers::FObjectFinder<UDataTable> clothesTable_BP(TEXT("DataTable'/Game/Data/Tables/ClothesDataTable.ClothesDataTable'"));
	_clothesTable = clothesTable_BP.Object;
	ConstructorHelpers::FObjectFinder<UDataTable> consumableTable_BP(TEXT("DataTable'/Game/Data/Tables/ConsumableDataTable.ConsumableDataTable'"));
	_consumableTable = consumableTable_BP.Object;

	Init();
	ACTIVE_DEBUG = false;
}

void AKnightPlayer::BeginPlay()
{
	Super::BeginPlay();

	LoadObjects();
	EquipWeapons();
	EquipClothes();
	EquipConsumable();
}

//////////////////////////////////////////////////////////////////////////
// Input

void AKnightPlayer::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	// Set up gameplay key bindings
	check(PlayerInputComponent);
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);

	PlayerInputComponent->BindAxis("MoveForward", this, &AKnightPlayer::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &AKnightPlayer::MoveRight);

	PlayerInputComponent->BindAction("Run", IE_Pressed, this, &AKnightPlayer::Run);
	PlayerInputComponent->BindAction("Run", IE_Released, this, &AKnightPlayer::StopRun);

	PlayerInputComponent->BindAction("Attack", IE_Pressed, this, &AKnightPlayer::Attack);

	PlayerInputComponent->BindAction("Action", IE_Pressed, this, &AKnightPlayer::Action);
	PlayerInputComponent->BindAction("Cancel", IE_Pressed, this, &AKnightPlayer::Cancel);

	PlayerInputComponent->BindAction("Heal", IE_Pressed, this, &AKnightPlayer::Heal);

	// We have 2 versions of the rotation bindings to handle different kinds of devices differently
	// "turn" handles devices that provide an absolute delta, such as a mouse.
	// "turnrate" is for devices that we choose to treat as a rate of change, such as an analog joystick
	PlayerInputComponent->BindAxis("Turn", this, &AKnightPlayer::Turn);
	PlayerInputComponent->BindAxis("TurnRate", this, &AKnightPlayer::TurnAtRate);
	PlayerInputComponent->BindAxis("LookUp", this, &AKnightPlayer::LookUp);
	PlayerInputComponent->BindAxis("LookUpRate", this, &AKnightPlayer::LookUpAtRate);
}

void AKnightPlayer::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (CanMove())
	{
		if (_isMovingForward || _isMovingRight)
		{
			if (_currentPlayerState != EPlayerState::PS_Run)
			{
				_currentPlayerState = EPlayerState::PS_Walk;
			}
			else if (_isRunning)
			{
				_currentPlayerState = EPlayerState::PS_Run;
			}
		}
		else
		{
			_currentPlayerState = EPlayerState::PS_Idle;
		}
	}

	if(_currentPlayerState == EPlayerState::PS_Run)
	{
		_currentStamina -= DeltaTime * _staminaSpeedLoose;
		_lastStaminaLost = 0.0f;
		if(_currentStamina <= 0)
		{
			_currentStamina = 0;
			StopRun();
		}
	}
	else if(_currentStamina < _maxStamina && _lastStaminaLost >= _timeToGainStamina)
	{
		_currentStamina += DeltaTime * _staminaSpeedGain;
	}
	else
	{
		_lastStaminaLost += DeltaTime;
	}
}

void AKnightPlayer::Init()
{
	_currentPlayerState = EPlayerState::PS_Idle;

	_maxLife = 100;
	_currentLife = _maxLife;

	_maxStamina = 50.0f;
	_currentStamina = _maxStamina;
	_lastStaminaLost = 0.0f;
	_timeToGainStamina = 1.5f;

	_isMovingForward = false;
	_isMovingRight = false;
	_isRunning = false;

	_walkingSpeed = 300.0f;
	_runningSpeed = 600.0f;
	_staminaSpeedLoose = 2.0f;
	_staminaSpeedGain = 2.0f;
	GetCharacterMovement()->MaxWalkSpeed = _walkingSpeed;

	_maxWeight = 100;
	_totalWeight = 0;
	
	_gold = 50;
	_shopOpen = false;
}

void AKnightPlayer::Run()
{
	if (_currentStamina > 0 && _currentPlayerState == EPlayerState::PS_Walk)
	{
		_isRunning = true;
		_currentPlayerState = EPlayerState::PS_Run;
		GetCharacterMovement()->MaxWalkSpeed = _runningSpeed;
	}
}

void AKnightPlayer::StopRun()
{
	_isRunning = false;
	_currentPlayerState = EPlayerState::PS_Walk;
	GetCharacterMovement()->MaxWalkSpeed = _walkingSpeed;
}

void AKnightPlayer::TurnAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerYawInput(Rate * _baseTurnRate * GetWorld()->GetDeltaSeconds());
}

void AKnightPlayer::LookUpAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerPitchInput(Rate * _baseLookUpRate * GetWorld()->GetDeltaSeconds());
}

void AKnightPlayer::Action()
{
	if (_sellerToOpen && !_shopOpen)
	{
		UE_LOG(LogTemp, Warning, TEXT("OPEN SELLER"));
		_shopOpen = true;
		OpenShop(_sellerToOpen);
	}
}

void AKnightPlayer::Cancel()
{
	if (_sellerToOpen && _shopOpen)
	{
		UE_LOG(LogTemp, Warning, TEXT("CLOSE SELLER"));
		_shopOpen = false;
		HideShop();
	}
}

bool AKnightPlayer::CanAttack() const
{
	if(_currentPlayerState == EPlayerState::PS_Attack ||
		_currentPlayerState == EPlayerState::PS_Heal ||
		_currentStamina < _staminaAttackLoose)
	{
		if(ACTIVE_DEBUG)
		{
			UE_LOG(LogTemp, Warning, TEXT("CAN'T ATTACK"));
			UE_LOG(LogTemp, Warning, TEXT("PS_Attack %s"), _currentPlayerState == EPlayerState::PS_Attack ? TEXT("true") : TEXT("false"));
			UE_LOG(LogTemp, Warning, TEXT("PS_Heal %s"), _currentPlayerState == EPlayerState::PS_Heal ? TEXT("true") : TEXT("false"));
			UE_LOG(LogTemp, Warning, TEXT("stamina %s"), _currentStamina < _staminaAttackLoose ? TEXT("true") : TEXT("false"));
		}
		return false;
	}
	else if (_shopOpen)
	{
		return false;
	}
	return true;
}

void AKnightPlayer::Attack()
{
	if(CanAttack())
	{
		if (_firstWeapon)
		{
			_firstWeapon->EmptyActorHit();
		}
		_currentPlayerState = EPlayerState::PS_Attack;
		_lastStaminaLost = 0.0f;
		_currentStamina -= _staminaAttackLoose;
	}
}

bool AKnightPlayer::CanHeal() const
{
	if (_currentPlayerState == EPlayerState::PS_Idle)
	{
		return true;
	}
	return false;
}

void AKnightPlayer::Heal()
{
	if(CanHeal())
	{
		_currentPlayerState = EPlayerState::PS_Heal;
	}
}

bool AKnightPlayer::CanMove() const
{
	if (_currentPlayerState == EPlayerState::PS_Attack ||
		_currentPlayerState == EPlayerState::PS_Heal)
	{
		return false;
	}
	else if (_shopOpen)
	{
		return false;
	}
	return true;
}

void AKnightPlayer::MoveForward(float Value)
{
	if ((Controller != nullptr) && (Value != 0.0f) && CanMove())
	{
		_isMovingForward = true;
		// find out which way is forward
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get forward vector
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		AddMovementInput(Direction, Value);
	}
	else
	{
		_isMovingForward = false;
	}
}

void AKnightPlayer::MoveRight(float Value)
{
	if ((Controller != nullptr) && (Value != 0.0f) && CanMove())
	{
		_isMovingRight = true;
		// find out which way is right
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get right vector 
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
		// add movement in that direction
		AddMovementInput(Direction, Value);
	}
	else
	{
		_isMovingRight = false;
	}
}

void AKnightPlayer::Turn(float Value)
{
	if (!_shopOpen)
	{
		AddControllerYawInput(Value);
	}
}

void AKnightPlayer::LookUp(float Value)
{
	if (!_shopOpen)
	{
		AddControllerPitchInput(Value);
	}
}

int AKnightPlayer::GetMaxLife() const
{
	return _maxLife;
}

int AKnightPlayer::GetcurrentLife() const
{
	return _currentLife;
}

float AKnightPlayer::GetMaxStamina() const
{
	return _maxStamina;
}

float AKnightPlayer::GetcurrentStamina() const
{
	return _currentStamina;
}

EPlayerState AKnightPlayer::GetCurrentPlayerState() const
{
	return _currentPlayerState;
}

void AKnightPlayer::SetCurrentPlayerState(EPlayerState NewPlayerState)
{
	_currentPlayerState = NewPlayerState;
}

void AKnightPlayer::PlayerTakeDamage(int amount)
{
	_currentLife -= amount;
	if (_currentLife <= 0)
	{
		Die();
	}
}

AKnightWeapon* AKnightPlayer::GetWeaponOne()
{
	return _firstWeapon;
}

AKnightWeapon* AKnightPlayer::GetWeaponTwo()
{
	return _secondWeapon;
}

AKnightClothes* AKnightPlayer::GetHelmet()
{
	return _helmet;
}

AKnightClothes* AKnightPlayer::GetTorso()
{
	return _torso;
}

AKnightClothes* AKnightPlayer::GetGreaves()
{
	return _greaves;
}

AKnightClothes* AKnightPlayer::GetGloveRight()
{
	return _gloveRight;
}

AKnightClothes* AKnightPlayer::GetGloveLeft()
{
	return _gloveLeft;
}

AKnightClothes* AKnightPlayer::GetShoeRight()
{
	return _shoeRight;
}

AKnightClothes* AKnightPlayer::GetShoeLeft()
{
	return _shoeLeft;
}

TArray<FWeaponStruct> AKnightPlayer::GetWeapons()
{
	return _weapons;
}

TArray<FClothesStruct> AKnightPlayer::GetClothes()
{
	return _clothes;
}

TArray<FConsumableStruct> AKnightPlayer::GetConsummable()
{
	return _consumables;
}

TArray<FItemStruct> AKnightPlayer::GetItem()
{
	return _others;
}

void AKnightPlayer::Die()
{
	_currentPlayerState = EPlayerState::PS_Dead;
}

void AKnightPlayer::GetTotalWeight()
{
	_totalWeight = 0;

	_totalWeight += _firstWeapon->GetInfo().base.weight;
}

void AKnightPlayer::LoadObjects()
{
	// load all weapons on the player
	for (auto i=0; i < _weaponsToLoad.Num(); i++)
	{
		FString theName = FString::Printf(TEXT("%d"), _weaponsToLoad[i]);
		FWeaponStruct* rowWeapon = _weaponsTable->FindRow<FWeaponStruct>(FName(*theName), _weaponsTable->GetName(), true);

		if (rowWeapon)
		{
			int32 index = HaveWeapon(_weaponsToLoad[i]);
			if (index == -1)
				_weapons.Add(*rowWeapon);
			else
				_weapons[index].base.quantity++;
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("ROW NOT FOUND FOR WEAPON"));
		}
	}
	// load all clothes on the player
	for (auto i = 0; i < _clothesToLoad.Num(); i++)
	{
		FString theName = FString::Printf(TEXT("%d"), _clothesToLoad[i]);
		FClothesStruct* rowClothes = _clothesTable->FindRow<FClothesStruct>(FName(*theName), _clothesTable->GetName(), true);

		if (rowClothes)
		{
			int32 index = HaveClothes(_clothesToLoad[i]);
			if (index == -1)
				_clothes.Add(*rowClothes);
			else
				_clothes[index].base.quantity++;
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("ROW NOT FOUND FOR CLOTHES"));
		}
	}
	// load all consumable on the player
	for (auto i = 0; i < _consumablesToLoad.Num(); i++)
	{
		FString theName = FString::Printf(TEXT("%d"), _consumablesToLoad[i]);
		FConsumableStruct* rowConsumable = _consumableTable->FindRow<FConsumableStruct>(FName(*theName), _consumableTable->GetName(), true);

		if (rowConsumable)
		{
			int32 index = HaveConsumable(_consumablesToLoad[i]);
			if(index == -1)
				_consumables.Add(*rowConsumable);
			else
				_consumables[index].base.quantity++;
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("ROW NOT FOUND FOR CONSUMABLE"));
		}
	}
}

int32 AKnightPlayer::HaveWeapon(int32 id)
{
	for (auto i = 0; i < _weapons.Num(); i++)
	{
		if (_weapons[i].base.id == id)
			return i;
	}
	return -1;
}

int32 AKnightPlayer::HaveConsumable(int32 id)
{
	for (auto i = 0; i < _consumables.Num(); i++)
	{
		if (_consumables[i].base.id == id)
			return i;
	}
	return -1;
}

int32 AKnightPlayer::HaveClothes(int32 id)
{
	for (auto i = 0; i < _clothes.Num(); i++)
	{
		if (_clothes[i].base.id == id)
			return i;
	}
	return -1;
}

void AKnightPlayer::OnPlayerDetectionOverlapBegin(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult)
{
	AKnightSeller* newSeller = Cast<AKnightSeller>(OtherActor);
	if (newSeller)
	{
		_sellerToOpen = newSeller;
	}
}

void AKnightPlayer::OnPlayerDetectionOverlapEnd(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (OtherActor == _sellerToOpen)
	{
		_sellerToOpen = nullptr;
	}
}

void AKnightPlayer::EquipWeapons()
{
	int weaponsNumber = _weapons.Num();
	if (weaponsNumber > 0)
	{
		_firstWeapon = GetWorld()->SpawnActor<AKnightWeapon>(_weapons[0].base.blueprint, GetActorLocation(), GetActorRotation());
		_firstWeapon->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, FName("RightWeaponSocket"));
		_firstWeapon->SetInfo(_weapons[0]);
	}
	if (weaponsNumber > 1)
	{
		_secondWeapon = GetWorld()->SpawnActor<AKnightWeapon>(_weapons[1].base.blueprint, GetActorLocation(), GetActorRotation());
		_secondWeapon->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, FName("LeftWeaponSocket"));
		_secondWeapon->SetInfo(_weapons[1]);
	}
}

void AKnightPlayer::EquipClothes()
{
	for (int i = 0; i < _clothes.Num(); i++)
	{
		switch (_clothes[i].clothesType)
		{
			case EClothesTypeEnum::Helmet :
			{
				_helmet = GetWorld()->SpawnActor<AKnightClothes>(_clothes[i].base.blueprint, GetActorLocation(), GetActorRotation());
				_helmet->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, FName("HeadSocket"));
				_helmet->SetInfo(_clothes[i]);
				break;
			}
			case EClothesTypeEnum::Torso:
			{
				_torso = GetWorld()->SpawnActor<AKnightClothes>(_clothes[i].base.blueprint, GetActorLocation(), GetActorRotation());
				_torso->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, FName("TorsoSocket"));
				_torso->SetInfo(_clothes[i]);
				break;
			}
			case EClothesTypeEnum::Greaves:
			{
				_greaves = GetWorld()->SpawnActor<AKnightClothes>(_clothes[i].base.blueprint, GetActorLocation(), GetActorRotation());
				_greaves->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, FName("GreavesSocket"));
				_greaves->SetInfo(_clothes[i]);
				break;
			}
			case EClothesTypeEnum::Hand:
			{
				_gloveRight = GetWorld()->SpawnActor<AKnightClothes>(_clothes[i].base.blueprint, GetActorLocation(), GetActorRotation());
				_gloveRight->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, FName("RightHandSocket"));
				_gloveRight->SetInfo(_clothes[i]);
				_gloveLeft = GetWorld()->SpawnActor<AKnightClothes>(_clothes[i].base.blueprint, GetActorLocation(), GetActorRotation());
				_gloveLeft->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, FName("LeftHandSocket"));
				_gloveLeft->SetInfo(_clothes[i]);
				break;
			}
			case EClothesTypeEnum::Foot:
			{
				_shoeLeft = GetWorld()->SpawnActor<AKnightClothes>(_clothes[i].base.blueprint, GetActorLocation(), GetActorRotation());
				_shoeLeft->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, FName("LeftFootSocket"));
				_shoeLeft->SetInfo(_clothes[i]);
				_shoeRight = GetWorld()->SpawnActor<AKnightClothes>(_clothes[i].base.blueprint, GetActorLocation(), GetActorRotation());
				_shoeRight->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, FName("RightFootSocket"));
				_shoeRight->SetInfo(_clothes[i]);
				break;
			}
			case EClothesTypeEnum::Ring:
			{
				if (_firstRing == nullptr)
				{
					_firstRing = GetWorld()->SpawnActor<AKnightClothes>(_clothes[i].base.blueprint, GetActorLocation(), GetActorRotation());
					_firstRing->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, FName("FirstRingSocket"));
					_firstRing->SetInfo(_clothes[i]);
				}
				else if (_secondRing == nullptr)
				{
					_secondRing = GetWorld()->SpawnActor<AKnightClothes>(_clothes[i].base.blueprint, GetActorLocation(), GetActorRotation());
					_secondRing->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, FName("SecondRingSocket"));
					_secondRing->SetInfo(_clothes[i]);
				}
				break;
			}
		}
	}
}

void AKnightPlayer::EquipConsumable()
{
	int consumableNumber = _consumables.Num();
	if (consumableNumber > 0)
	{
		_firstSlot = GetWorld()->SpawnActor<AKnightConsumable>(_consumables[0].base.blueprint, GetActorLocation(), GetActorRotation());
		_firstSlot->SetInfo(_consumables[0]);
	}
	if (consumableNumber > 1)
	{
		_secondSlot = GetWorld()->SpawnActor<AKnightConsumable>(_consumables[1].base.blueprint, GetActorLocation(), GetActorRotation());
		_secondSlot->SetInfo(_consumables[1]);
	}
}