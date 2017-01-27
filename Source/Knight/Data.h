#pragma once

#include "Data.generated.h"

class AKnightObject;

UENUM(BlueprintType)
enum class EItemTypeEnum : uint8
{
	None 		UMETA(DisplayName = "None"),
	Weapon 		UMETA(DisplayName = "Weapon"),
	Stuff 		UMETA(DisplayName = "Stuff"),
	Usable		UMETA(DisplayName = "Usable"),
};

UENUM(BlueprintType)
enum class EClothesTypeEnum : uint8
{
	None 		UMETA(DisplayName = "None"),
	Helmet 		UMETA(DisplayName = "Helmet"),
	Chest 		UMETA(DisplayName = "Chest"),
	RightHand	UMETA(DisplayName = "RightHand"),
	LeftHand	UMETA(DisplayName = "LeftHand"),
	Greaves		UMETA(DisplayName = "Greaves"),
	Foot		UMETA(DisplayName = "Foot"),
};

USTRUCT(BlueprintType)
struct FItemStruct
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item Info")
	int32 id;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item Info")
	int32 quantity;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item Info")
	UTexture2D* image;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item Info")
	EItemTypeEnum itemType;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item Info")
	FString name;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item Info")
	int32 weight;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item Info")
	int32 value;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item Info")
	TSubclassOf<AKnightObject> blueprint;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item Info")
	int32 durabilityMax;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item Info")
	int32 currentDurability;

	//Constructor
	FItemStruct()
	{
		id = -1;
		quantity = 1;
		image = nullptr;
		itemType = EItemTypeEnum::None;
		name = "UNKNOWN";
		weight = 0;
		value = 1;
		durabilityMax = 0;
		currentDurability = 0;
	}
};

USTRUCT(BlueprintType)
struct FWeaponStruct : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item Info")
	FItemStruct base;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item Info")
	int32 damage;

	FWeaponStruct()
	{
		damage = 1;
	}
};

USTRUCT(BlueprintType)
struct FClothesStruct : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item Info")
	FItemStruct base;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item Info")
	int32 armor;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item Info")
	EClothesTypeEnum clothesType;

	//Constructor
	FClothesStruct()
	{
		armor = 1;
		clothesType = EClothesTypeEnum::None;
	}
};

USTRUCT(BlueprintType)
struct FConsumableStruct : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item Info")
	FItemStruct base;

	// put effect here
	//UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item Info")
	//int32 armor;

	//Constructor
	FConsumableStruct()
	{
		base.weight = 0;
		
	}
};