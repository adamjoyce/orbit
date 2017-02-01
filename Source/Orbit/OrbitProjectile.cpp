// Copyright 1998-2016 Epic Games, Inc. All Rights Reserve

#include "Orbit.h"
#include "OrbitProjectile.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Planet.h"
#include "EngineUtils.h"
#include "Kismet/KismetMathLibrary.h"

#include <EngineGlobals.h>
#include <Runtime/Engine/Classes/Engine/Engine.h>

AOrbitProjectile::AOrbitProjectile() 
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Static reference to the mesh to use for the projectile
	static ConstructorHelpers::FObjectFinder<UStaticMesh> ProjectileMeshAsset(TEXT("/Game/TwinStick/Meshes/TwinStickProjectile.TwinStickProjectile"));

	// Create mesh component for the projectile sphere
	ProjectileMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ProjectileMesh0"));
	ProjectileMesh->SetStaticMesh(ProjectileMeshAsset.Object);
	ProjectileMesh->SetupAttachment(RootComponent);
	ProjectileMesh->BodyInstance.SetCollisionProfileName("Projectile");
	ProjectileMesh->OnComponentHit.AddDynamic(this, &AOrbitProjectile::OnHit);		// set up a notification for when this component hits something
	RootComponent = ProjectileMesh;

	// Use a ProjectileMovementComponent to govern this projectile's movement
	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovement0"));
	ProjectileMovement->UpdatedComponent = ProjectileMesh;
	ProjectileMovement->InitialSpeed = 3000.f;
	ProjectileMovement->MaxSpeed = 3000.f;
	ProjectileMovement->bRotationFollowsVelocity = false;
	ProjectileMovement->bShouldBounce = false;
	ProjectileMovement->ProjectileGravityScale = 0.f; // No gravity
	ProjectileMovement->bConstrainToPlane = false;

	// Die after 3 seconds by default
	InitialLifeSpan = 2.0f;
}

void AOrbitProjectile::BeginPlay()
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
	//GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Blue, TEXT("" + CurrentPlanet->GetName()));
}

void AOrbitProjectile::Tick(float DeltaSeconds)
{
	// Update planet variables.
	TArray<FVector> PlanetNormalAndShipDistance;
	PlanetNormalAndShipDistance = CurrentPlanet->GetSurfaceNormalAndObjectDistance(GetActorLocation());
	//GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Blue, FString::Printf(TEXT("Normal: %f, %f, %f"), PlanetNormalAndShipDistance[1].X, PlanetNormalAndShipDistance[1].Y, PlanetNormalAndShipDistance[1].Z));

	// Glue projectile to planet.
	SetActorLocation(PlanetNormalAndShipDistance[1]);
	SetActorRotation(UKismetMathLibrary::MakeRotFromZX(PlanetNormalAndShipDistance[0], GetActorForwardVector()));
	ProjectileMovement->Velocity = GetActorForwardVector() * ProjectileMovement->MaxSpeed;
	//GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Blue, FString::Printf(TEXT("Velocity: %f, %f, %f"), ProjectileMovement->Velocity.X, ProjectileMovement->Velocity.Y, ProjectileMovement->Velocity.Z));
}

void AOrbitProjectile::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	// Only add impulse and destroy projectile if we hit a physics
	if ((OtherActor != NULL) && (OtherActor != this) && (OtherComp != NULL) && OtherComp->IsSimulatingPhysics())
	{
		OtherComp->AddImpulseAtLocation(GetVelocity() * 20.0f, GetActorLocation());
	}

	Destroy();
	//GEngine->AddOnScreenDebugMessage(1, 15.0f, FColor::Blue, TEXT("Destroyed"));
}