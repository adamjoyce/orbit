// Fill out your copyright notice in the Description page of Project Settings.

#include "Orbit.h"
#include "OrbitEnemy.h"
#include "Planet.h"
#include "EngineUtils.h"
#include "Kismet/KismetMathLibrary.h"


// Sets default values
AOrbitEnemy::AOrbitEnemy()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Create the actor's mesh.
	const ConstructorHelpers::FObjectFinder<UStaticMesh> EnemyMesh(TEXT("StaticMesh'/Game/TwinStick/Meshes/EnemyUFO.EnemyUFO'"));
	EnemyMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("EnemyMesh"));
	EnemyMeshComponent->SetCollisionProfileName(UCollisionProfile::Pawn_ProfileName);
	EnemyMeshComponent->SetStaticMesh(EnemyMesh.Object);
	RootComponent = EnemyMeshComponent;

	MoveSpeed = 500.0f;
}

// Called when the game starts or when spawned
void AOrbitEnemy::BeginPlay()
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
}

// Called every frame
void AOrbitEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// Choose a random direction vector to move in.
	FVector MoveDirection = FMath::VRandCone(GetActorForwardVector(), 0.1);
	MoveDirection = FVector(MoveDirection.X, MoveDirection.Y, 0);


	// Move the actor.
	FVector Movement = MoveDirection * MoveSpeed * DeltaTime;
	FRotator NewRotation = MoveDirection.Rotation();
	FHitResult Hit(1.f);
	//AddActorLocalOffset(Movement, true, &Hit);
	RootComponent->MoveComponent(Movement, NewRotation, true, &Hit);

	// Update planet variables.
	TArray<FVector> PlanetNormalAndShipDistance;
	PlanetNormalAndShipDistance = CurrentPlanet->GetSurfaceNormalAndObjectDistance(GetActorLocation());
	//GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Blue, FString::Printf(TEXT("Normal: %f, %f, %f"), PlanetNormalAndShipDistance[0].X, PlanetNormalAndShipDistance[0].Y, PlanetNormalAndShipDistance[0].Z));

	// Glue enemy to the planet with the correct rotation.
	SetActorLocation(PlanetNormalAndShipDistance[1]);
	SetActorRotation(UKismetMathLibrary::MakeRotFromZX(PlanetNormalAndShipDistance[0], GetActorForwardVector()));
	//EnemyMeshComponent->SetRelativeRotation(GetActorForwardVector().Rotation());
	//FRotator(ShipMeshComponent->GetComponentRotation().Roll, ShipMeshComponent->GetComponentRotation().Pitch, PlanetNormalAndShipDistance[0].Z)
}

