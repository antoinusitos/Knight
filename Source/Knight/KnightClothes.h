// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "KnightObject.h"
#include "KnightClothes.generated.h"

/**
 * 
 */
UCLASS()
class KNIGHT_API AKnightClothes : public AKnightObject
{
	GENERATED_BODY()
	
protected:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "clothes Info")
	FClothesStruct _dataInfo;

public:

	void Equip();

	void UnEquip();
	
	FClothesStruct GetInfo();
	void SetInfo(const FClothesStruct& infos);
};
