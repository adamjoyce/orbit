// Fill out your copyright notice in the Description page of Project Settings.

#include "Orbit.h"
#include "Planet.h"


// Sets default values
APlanet::APlanet()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	// Setup the planet collision sphere.
	CoreComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Core"));
	RootComponent = CoreComponent;

	// Planet radius variables.
	RadiusInMeters = 15.0f;
	RadiusInUnits = RadiusInMeters * 100.0f;

	// Setup the planet mesh.
	PlanetMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("PlanetMesh"));
	static ConstructorHelpers::FObjectFinder<UStaticMesh> PlanetVisualAsset(TEXT("/Game/StarterContent/Shapes/Shape_Sphere.Shape_Sphere"));
	if (PlanetVisualAsset.Succeeded())
	{
		PlanetMeshComponent->SetStaticMesh(PlanetVisualAsset.Object);
		PlanetScale = RadiusInUnits / PlanetMeshComponent->Bounds.BoxExtent.X;
		PlanetMeshComponent->SetWorldScale3D(FVector(PlanetScale));
		PlanetMeshComponent->SetRelativeLocation(FVector(0.0f, 0.0f, -RadiusInUnits));
		PlanetMeshComponent->SetupAttachment(RootComponent);
	}
}

// Called when the game starts or when spawned
void APlanet::BeginPlay()
{
	Super::BeginPlay();
	PlanetMeshComponent->SetWorldScale3D(FVector(PlanetScale));
	PlanetMeshComponent->SetRelativeLocation(FVector(0.0f, 0.0f, -RadiusInUnits));	
}

//
void APlanet::CalculateShipNormalAndSurfaceDistance(const FVector ShipLocation)
{
	// The normal at the planet location below the ship.
	FVector ShipSurfaceNormal = ShipLocation - GetActorLocation();
	ShipSurfaceNormal.Normalize();

	// The distance from the planet centre to the ship.
	ShipPlanetDistance = (ShipSurfaceNormal * RadiusInUnits) + GetActorLocation();
}

//// Called every frame
//void APlanet::Tick( float DeltaTime )
//{
//	Super::Tick( DeltaTime );
//
//}

