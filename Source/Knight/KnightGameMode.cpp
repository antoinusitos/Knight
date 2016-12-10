// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.

#include "Knight.h"
#include "KnightGameMode.h"
#include "KnightPlayer.h"

AKnightGameMode::AKnightGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/Blueprints/BP_KnightPlayer"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}
