// Fill out your copyright notice in the Description page of Project Settings.

#include "Orbit.h"
#include "Planet.h"


// Sets default values
APlanet::APlanet()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	// Setup the planet collision sphere.
	/*CoreComponent = CreateDefaultSubobject<UBillboardComponent>(TEXT("Core"));
	RootComponent = CoreComponent;*/

	// Setup the planet mesh.
	PlanetMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("PlanetMesh"));
	static ConstructorHelpers::FObjectFinder<UStaticMesh> PlanetVisualAsset(TEXT("/Game/StarterContent/Shapes/Shape_Sphere.Shape_Sphere"));
	if (PlanetVisualAsset.Succeeded())
	{
		PlanetMeshComponent->SetStaticMesh(PlanetVisualAsset.Object);
		//PlanetMeshComponent->SetRelativeLocation(FVector(0.0f, 0.0f, -40.0f));
		PlanetMeshComponent->SetupAttachment(RootComponent);
		//RootComponent = PlanetMeshComponent;
	}

	// Scale the planet.
	/*RadiusInMeters = 10.0f;
	RadiusInUnits = RadiusInMeters * 100.0f;
	float PlanetScale = RadiusInUnits / PlanetMeshComponent->Bounds.BoxExtent.X;
	PlanetMeshComponent->SetWorldScale3D(FVector(PlanetScale));*/
}

// Called when the game starts or when spawned
void APlanet::BeginPlay()
{
	Super::BeginPlay();
	
}

//// Called every frame
//void APlanet::Tick( float DeltaTime )
//{
//	Super::Tick( DeltaTime );
//
//}

