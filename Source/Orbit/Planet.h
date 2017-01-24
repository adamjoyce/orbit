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
	class USphereComponent* PlanetSphereComponent;

	// The planet mesh component.
	UPROPERTY()
	class UStaticMeshComponent* PlanetMeshComponent;

	// Planet radius in UE4 units (cm).
	UPROPERTY()
	float RadiusInUnits;
	
public:	
	// Sets default values for this actor's properties
	APlanet();

	// Planet radius in meters.
	UPROPERTY(Category = PlanetVariables, EditAnywhere, BlueprintReadWrite)
	float RadiusInMeters;

	//Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	// Called every frame
	//virtual void Tick( float DeltaSeconds ) override;
};
