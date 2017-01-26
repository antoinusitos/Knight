// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "Data.h"
#include "KnightObject.h"
#include "KnightWeapon.generated.h"

UCLASS()
class KNIGHT_API AKnightWeapon : public AKnightObject
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AKnightWeapon();

	// Empty all hit actors when attacking
	void EmptyActorHit();

	FWeaponStruct GetInfo();
	void SetInfo(const FWeaponStruct& infos);

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon Mesh")
	USkeletalMeshComponent* _mesh;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon Collisions")
	UBoxComponent* _collisions;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon Info")
	FWeaponStruct _dataInfo;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon Info")
	FName _dataTableID;

private:
	UFUNCTION()
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	
	// Initialize variables
	void Init();

	TArray<AActor*> _actorsHit;

};
