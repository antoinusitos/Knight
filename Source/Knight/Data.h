#pragma once

#include "Data.generated.h"

UENUM(BlueprintType)		//"BlueprintType" is essential to include
enum class EItemTypeEnum : uint8
{
	None 		UMETA(DisplayName = "None"),
	Weapon 		UMETA(DisplayName = "Weapon"),
	Stuff 		UMETA(DisplayName = "Stuff"),
	Usable		UMETA(DisplayName = "Usable"),
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

	//Constructor
	FItemStruct()
	{
		id = -1;
		quantity = 1;
		image = nullptr;
		itemType = EItemTypeEnum::None;
	}
};