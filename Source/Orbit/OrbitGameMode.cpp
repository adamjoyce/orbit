// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.

#include "Orbit.h"
#include "OrbitGameMode.h"
#include "OrbitPawn.h"

AOrbitGameMode::AOrbitGameMode()
{
	// set default pawn class to our character class
	DefaultPawnClass = AOrbitPawn::StaticClass();
}

