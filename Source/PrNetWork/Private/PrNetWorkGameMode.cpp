// Copyright Epic Games, Inc. All Rights Reserved.

#include "PrNetWorkGameMode.h"
#include "PrNetWorkCharacter.h"
#include "UObject/ConstructorHelpers.h"

APrNetWorkGameMode::APrNetWorkGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPerson/Blueprints/BP_ThirdPersonCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}
