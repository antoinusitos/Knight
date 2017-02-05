// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "Data.h"
#include "KnightSeller.generated.h"

class AKnightWeapon;
class AKnightClothes;
class AKnightConsumable;

UCLASS()
class KNIGHT_API AKnightSeller : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AKnightSeller();

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	// Called every frame
	virtual void Tick( float DeltaSeconds ) override;

protected:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Seller Inventory")
		TArray<int32> _weaponsToLoad;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Seller Inventory")
		TArray<int32> _clothesToLoad;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Seller Inventory")
		TArray<int32> _consumablesToLoad;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Seller Inventory")
		TArray<int32> _othersToLoad;

private:
	// INVENTORY

	UPROPERTY(VisibleAnywhere, Category = "Seller Inventory")
		TArray<FWeaponStruct> _weapons;

	UPROPERTY(VisibleAnywhere, Category = "Seller Inventory")
		TArray<FClothesStruct> _clothes;

	UPROPERTY(VisibleAnywhere, Category = "Seller Inventory")
		TArray<FConsumableStruct> _consumables;

	UPROPERTY(VisibleAnywhere, Category = "Seller Inventory")
		TArray<FItemStruct> _others;

	UDataTable* _weaponsTable;
	UDataTable* _clothesTable;
	UDataTable* _consumableTable;

	int32 HaveWeapon(int32 id);
	int32 HaveConsumable(int32 id);
	int32 HaveClothes(int32 id);

	void LoadObjects();
};
