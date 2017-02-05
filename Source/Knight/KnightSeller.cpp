// Fill out your copyright notice in the Description page of Project Settings.

#include "Knight.h"
#include "KnightSeller.h"
#include "KnightWeapon.h"
#include "KnightClothes.h"
#include "KnightConsumable.h"

// Sets default values
AKnightSeller::AKnightSeller()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	ConstructorHelpers::FObjectFinder<UDataTable> weaponTable_BP(TEXT("DataTable'/Game/Data/Tables/WeaponDataTable.WeaponDataTable'"));
	_weaponsTable = weaponTable_BP.Object;
	ConstructorHelpers::FObjectFinder<UDataTable> clothesTable_BP(TEXT("DataTable'/Game/Data/Tables/ClothesDataTable.ClothesDataTable'"));
	_clothesTable = clothesTable_BP.Object;
	ConstructorHelpers::FObjectFinder<UDataTable> consumableTable_BP(TEXT("DataTable'/Game/Data/Tables/ConsumableDataTable.ConsumableDataTable'"));
	_consumableTable = consumableTable_BP.Object;
}

// Called when the game starts or when spawned
void AKnightSeller::BeginPlay()
{
	Super::BeginPlay();
	
	LoadObjects();
}

// Called every frame
void AKnightSeller::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );

}

void AKnightSeller::LoadObjects()
{
	// load all weapons on the player
	for (auto i = 0; i < _weaponsToLoad.Num(); i++)
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
			if (index == -1)
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

int32 AKnightSeller::HaveWeapon(int32 id)
{
	for (auto i = 0; i < _weapons.Num(); i++)
	{
		if (_weapons[i].base.id == id)
			return i;
	}
	return -1;
}

int32 AKnightSeller::HaveConsumable(int32 id)
{
	for (auto i = 0; i < _consumables.Num(); i++)
	{
		if (_consumables[i].base.id == id)
			return i;
	}
	return -1;
}

int32 AKnightSeller::HaveClothes(int32 id)
{
	for (auto i = 0; i < _clothes.Num(); i++)
	{
		if (_clothes[i].base.id == id)
			return i;
	}
	return -1;
}