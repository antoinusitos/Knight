// Fill out your copyright notice in the Description page of Project Settings.

#include "Knight.h"
#include "KnightWeapon.h"
#include "KnightAI.h"


// Sets default values
AKnightWeapon::AKnightWeapon()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	_mesh = CreateDefaultSubobject<USkeletalMeshComponent>(FName("Mesh"));
	RootComponent = _mesh;

	_collisions = CreateDefaultSubobject<UBoxComponent>(FName("Collision"));
	_collisions->SetupAttachment(_mesh);
	
	// set up a notification for when this component overlaps something  
	_collisions->OnComponentBeginOverlap.AddDynamic(this, &AKnightWeapon::OnOverlapBegin);

	EmptyActorHit();

	Init();
}

void AKnightWeapon::EmptyActorHit()
{
	_actorsHit.Empty();
}

void AKnightWeapon::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	// Other Actor is the actor that triggered the event. Check that is not ourself.  
	if ((OtherActor != nullptr) && (OtherActor != this) && (OtherComp != nullptr))
	{
		auto ai = Cast<AKnightAI>(OtherActor);
		if(ai)
		{
			if (!_actorsHit.Contains(OtherActor))
			{
				_actorsHit.Add(OtherActor);
				UE_LOG(LogTemp, Warning, TEXT("send damage to AI"));
				ai->ReceiveDamage(_dataInfo.damage);
			}
		}
	}
}

void AKnightWeapon::Init()
{
	_dataTableID = "";
}

FWeaponStruct AKnightWeapon::GetInfo()
{
	return _dataInfo;
}

void AKnightWeapon::SetInfo(const FWeaponStruct& infos)
{
	_dataInfo = infos;
}