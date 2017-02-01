// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.

#include "Orbit.h"
#include "OrbitPawn.h"
#include "OrbitProjectile.h"
#include "Planet.h"
#include "TimerManager.h"
#include <EngineGlobals.h>
#include <Runtime/Engine/Classes/Engine/Engine.h>
#include "EngineUtils.h"
#include "Kismet/KismetMathLibrary.h"

const FName AOrbitPawn::MoveForwardBinding("MoveForward");
const FName AOrbitPawn::MoveRightBinding("MoveRight");
const FName AOrbitPawn::FireForwardBinding("FireForward");
const FName AOrbitPawn::FireRightBinding("FireRight");

AOrbitPawn::AOrbitPawn()
{	
	// Setup the ship's root component.
	ShipRoot = CreateDefaultSubobject<UBoxComponent>(TEXT("ShipRoot"));
	ShipRoot->SetCollisionProfileName(UCollisionProfile::Pawn_ProfileName);
	RootComponent = ShipRoot;

	static ConstructorHelpers::FObjectFinder<UStaticMesh> ShipMesh(TEXT("/Game/TwinStick/Meshes/TwinStickUFO.TwinStickUFO"));
	// Create the mesh component
	ShipMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ShipMesh"));
	ShipMeshComponent->SetupAttachment(RootComponent);
	ShipMeshComponent->SetCollisionProfileName(UCollisionProfile::Pawn_ProfileName);
	ShipMeshComponent->SetStaticMesh(ShipMesh.Object);

	//ShipRoot->SetBoundsScale(ShipMeshComponent->BoundsScale);
	
	// Cache our sound effect
	static ConstructorHelpers::FObjectFinder<USoundBase> FireAudio(TEXT("/Game/TwinStick/Audio/TwinStickFire.TwinStickFire"));
	FireSound = FireAudio.Object;

	// Create a camera boom...
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 1200.f;
	CameraBoom->RelativeRotation = FRotator(-80.f, 0.f, 0.f);
	CameraBoom->bDoCollisionTest = false; // Don't want to pull camera in when it collides with level

	// Create a camera...
	CameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("TopDownCamera"));
	CameraComponent->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	CameraComponent->bUsePawnControlRotation = false;	// Camera does not rotate relative to arm

	// Movement
	MoveSpeed = 1000.0f;
	// Weapon
	GunOffset = FVector(90.f, 0.f, 0.f);
	FireRate = 0.1f;
	bCanFire = true;
}

void AOrbitPawn::SetupPlayerInputComponent(class UInputComponent* InputComponent)
{
	check(InputComponent);

	// set up gameplay key bindings
	InputComponent->BindAxis(MoveForwardBinding);
	InputComponent->BindAxis(MoveRightBinding);
	InputComponent->BindAxis(FireForwardBinding);
	InputComponent->BindAxis(FireRightBinding);
}

void AOrbitPawn::BeginPlay()
{
	Super::BeginPlay();
	// Find all planet's in the scene.
	TArray<APlanet*> Planets;
	for (TActorIterator<APlanet> ActorItr(GetWorld()); ActorItr; ++ActorItr)
	{
		// Will most likely need re-working when more planets are added...
		Planets.Add(*ActorItr);
	}

	if (Planets.Num() > 0)
		CurrentPlanet = Planets[0];
	//GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Blue, TEXT("" + CurrentPlanet->GetName()));
}

void AOrbitPawn::Tick(float DeltaSeconds)
{
	// Find movement direction
	const float ForwardValue = GetInputAxisValue(MoveForwardBinding);
	const float RightValue = GetInputAxisValue(MoveRightBinding);
	//GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Blue, FString::Printf(TEXT("Normal: %f"), ForwardValue));
	//GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Blue, FString::Printf(TEXT("Normal: %f"), RightValue));

	// Clamp max size so that (X=1, Y=1) doesn't cause faster movement in diagonal directions
	const FVector MoveDirection = FVector(ForwardValue, RightValue, 0.f).GetClampedToMaxSize(1.0f);
	//GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Blue, FString::Printf(TEXT("FowardValue: %f"), RightValue));

	// Calculate  movement
	const FVector Movement = MoveDirection * MoveSpeed * DeltaSeconds;

	// If non-zero size, move this actor
	if (Movement.SizeSquared() > 0.0f)
	{
		UGameplayStatics::SetGlobalTimeDilation(GetWorld(), 1.0f);
		const FRotator NewRotation = Movement.Rotation();
		FHitResult Hit(1.f);
		//RootComponent->MoveComponent(Movement, NewRotation, true, &Hit);
		AddActorLocalOffset(Movement, true, &Hit);

		// Update planet variables.
		TArray<FVector> PlanetNormalAndShipDistance;
		PlanetNormalAndShipDistance = CurrentPlanet->GetSurfaceNormalAndObjectDistance(GetActorLocation());
		//GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Blue, FString::Printf(TEXT("Normal: %f, %f, %f"), PlanetNormalAndShipDistance[0].X, PlanetNormalAndShipDistance[0].Y, PlanetNormalAndShipDistance[0].Z));

		// Glue ship to the planet with the correct rotation.
		SetActorLocation(PlanetNormalAndShipDistance[1]);
		SetActorRotation(UKismetMathLibrary::MakeRotFromZX(PlanetNormalAndShipDistance[0], GetActorForwardVector()));
		ShipMeshComponent->SetRelativeRotation(MoveDirection.Rotation());
		//FRotator(ShipMeshComponent->GetComponentRotation().Roll, ShipMeshComponent->GetComponentRotation().Pitch, PlanetNormalAndShipDistance[0].Z)
		
		if (Hit.IsValidBlockingHit())
		{
			GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Blue, TEXT("HIT"));
			const FVector Normal2D = Hit.Normal.GetSafeNormal();
			const FVector Deflection = FVector::VectorPlaneProject(Movement, Normal2D) * (1.f - Hit.Time);
			//RootComponent->MoveComponent(Deflection, NewRotation, true);
			AddActorLocalOffset(Deflection, true, &Hit);
		}
	}
	else
	{
		UGameplayStatics::SetGlobalTimeDilation(GetWorld(), 0.05f);
	}
	
	// Create fire direction vector.
	const float FireForwardValue = GetInputAxisValue(FireForwardBinding);
	const float FireRightValue = GetInputAxisValue(FireRightBinding);
	FVector FireDirection = FVector(FireForwardValue, FireRightValue, 0.f);
	FRotator Rot = GetActorRotation();
	FireDirection = Rot.RotateVector(FireDirection);

	// Try and fire a shot.
	FireShot(FireDirection);
}

void AOrbitPawn::FireShot(FVector FireDirection)
{
	// If we it's ok to fire again
	if (bCanFire == true)
	{
		// If we are pressing fire stick in a direction
		if (FireDirection.SizeSquared() > 0.0f)
		{
			const FRotator FireRotation = FireDirection.Rotation();
			// Spawn projectile at an offset from this pawn
			const FVector SpawnLocation = GetActorLocation() + FireRotation.RotateVector(GunOffset);

			UWorld* const World = GetWorld();
			if (World != NULL)
			{
				// spawn the projectile
				World->SpawnActor<AOrbitProjectile>(SpawnLocation, FireRotation);
				//GEngine->AddOnScreenDebugMessage(1, 15.0f, FColor::Blue, FString::Printf(TEXT("SpawnLocation: %f %f %f"), SpawnLocation.X, SpawnLocation.Y, SpawnLocation.Z));
				//GEngine->AddOnScreenDebugMessage(1, 15.0f, FColor::Blue, FString::Printf(TEXT("FireRotation: %f %f %f"), FireDirection.Rotation().Roll, FireDirection.Rotation().Pitch, FireDirection.Rotation().Yaw));
			}

			bCanFire = false;
			World->GetTimerManager().SetTimer(TimerHandle_ShotTimerExpired, this, &AOrbitPawn::ShotTimerExpired, FireRate);

			// try and play the sound if specified
			if (FireSound != nullptr)
			{
				UGameplayStatics::PlaySoundAtLocation(this, FireSound, GetActorLocation());
			}

			bCanFire = false;
		}
	}
}

void AOrbitPawn::ShotTimerExpired()
{
	bCanFire = true;
}

