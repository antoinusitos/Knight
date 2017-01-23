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

	UPROPERTY()
	int32 ID;

	UPROPERTY()
	UTexture2D* image;

	UPROPERTY()
	EItemTypeEnum type;

	//Constructor
	FItemStruct()
	{
		ID = -1;
		type = EItemTypeEnum::None;
	}
};