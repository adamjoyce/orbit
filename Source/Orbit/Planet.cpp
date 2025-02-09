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

	AtmosphereHeight = 50.0f;

	// Setup the planet mesh.
	PlanetMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("PlanetMesh"));
	static ConstructorHelpers::FObjectFinder<UStaticMesh> PlanetVisualAsset(TEXT("/Game/StarterContent/Shapes/Shape_Sphere.Shape_Sphere"));
	if (PlanetVisualAsset.Succeeded())
	{
		PlanetMeshComponent->SetStaticMesh(PlanetVisualAsset.Object);
		PlanetMeshComponent->SetCollisionProfileName(TEXT("OverlapAll"));
		PlanetScale = RadiusInUnits / PlanetMeshComponent->Bounds.BoxExtent.X;
		PlanetMeshComponent->SetWorldScale3D(FVector(PlanetScale));
		PlanetMeshComponent->SetRelativeLocation(FVector(0.0f, 0.0f, -RadiusInUnits));
		PlanetMeshComponent->bCastDynamicShadow = false;
		PlanetMeshComponent->SetupAttachment(RootComponent);
	}
}

// Called when the game starts or when spawned
void APlanet::BeginPlay()
{
	Super::BeginPlay();
	if (PlanetMeshComponent != NULL)
	{
		PlanetMeshComponent->SetWorldScale3D(FVector(PlanetScale));
		PlanetMeshComponent->SetRelativeLocation(FVector(0.0f, 0.0f, -RadiusInUnits));
	}
}

//
TArray<FVector> APlanet::GetSurfaceNormalAndObjectDistance(const FVector ShipLocation)
{
	TArray<FVector> results;

	// The normal at the planet location below the ship.
	FVector ShipSurfaceNormal = ShipLocation - GetActorLocation();
	ShipSurfaceNormal.Normalize();
	results.Add(ShipSurfaceNormal);

	// The distance from the planet centre to the ship.
	FVector ShipPlanetDistance = (ShipSurfaceNormal * (RadiusInUnits + AtmosphereHeight)) + GetActorLocation();
	results.Add(ShipPlanetDistance);

	return results;
}

//// Called every frame
//void APlanet::Tick( float DeltaTime )
//{
//	Super::Tick( DeltaTime );
//
//}

