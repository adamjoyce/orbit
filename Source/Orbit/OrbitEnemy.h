// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "OrbitEnemy.generated.h"

UCLASS()
class ORBIT_API AOrbitEnemy : public AActor
{
	GENERATED_BODY()

	UPROPERTY()
	class UStaticMeshComponent* EnemyMeshComponent;
	
public:	
	// Current planet the Actor is on.
	class APlanet* CurrentPlanet;

	// Move speed for the enemy.
	float MoveSpeed;

	// Sets default values for this actor's properties
	AOrbitEnemy();

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	// Called every frame
	virtual void Tick( float DeltaSeconds ) override;
};
