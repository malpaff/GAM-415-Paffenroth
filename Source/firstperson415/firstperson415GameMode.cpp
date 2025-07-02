// Copyright Epic Games, Inc. All Rights Reserved.

#include "firstperson415GameMode.h"
#include "firstperson415Character.h"
#include "UObject/ConstructorHelpers.h"

Afirstperson415GameMode::Afirstperson415GameMode()
	: Super()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnClassFinder(TEXT("/Game/FirstPerson/Blueprints/BP_FirstPersonCharacter"));
	DefaultPawnClass = PlayerPawnClassFinder.Class;

}
