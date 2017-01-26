// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "Planet.generated.h"

UCLASS()
class ORBIT_API APlanet : public AActor
{
	GENERATED_BODY()

	// The planet mesh component.
	UPROPERTY()
	class USceneComponent* CoreComponent;

	// The planet mesh component.
	UPROPERTY()
	class UStaticMeshComponent* PlanetMeshComponent;

	// Planet radius in UE4 units (cm).
	UPROPERTY()
	float RadiusInUnits;

	// The planet's 3d world scale.
	UPROPERTY()
		float PlanetScale;
	
public:	
	// Sets default values for this actor's properties
	APlanet();

	// Planet radius in meters.
	UPROPERTY(Category = PlanetVariables, EditAnywhere, BlueprintReadWrite)
	float RadiusInMeters;

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// Returns the object's distance from the planet's center and the surface normal below the object.
	TArray<FVector> GetSurfaceNormalAndObjectDistance(FVector ObjectLocation);
	
	// Called every frame
	//virtual void Tick( float DeltaSeconds ) override;
};
