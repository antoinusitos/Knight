// Fill out your copyright notice in the Description page of Project Settings.

#include "Knight.h"
#include "KnightClothes.h"

void AKnightClothes::Equip()
{
	UE_LOG(LogTemp, Warning, TEXT("%s was Equip"), *GetName());
}

void AKnightClothes::UnEquip()
{
	UE_LOG(LogTemp, Warning, TEXT("%s was UnEquip"), *GetName());
}
