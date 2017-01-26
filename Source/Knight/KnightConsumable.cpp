// Fill out your copyright notice in the Description page of Project Settings.

#include "Knight.h"
#include "KnightConsumable.h"

FConsumableStruct AKnightConsumable::GetInfo()
{
	return _dataInfo;
}

void AKnightConsumable::SetInfo(const FConsumableStruct& infos)
{
	_dataInfo = infos;
}