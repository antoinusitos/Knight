// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "KnightObject.h"
#include "KnightConsumable.generated.h"

/**
 * 
 */
UCLASS()
class KNIGHT_API AKnightConsumable : public AKnightObject
{
	GENERATED_BODY()
	
protected:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "clothes Info")
	FConsumableStruct _dataInfo;
	
public:
	FConsumableStruct GetInfo();
	void SetInfo(const FConsumableStruct& infos);
};
