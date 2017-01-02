// Fill out your copyright notice in the Description page of Project Settings.

#include "Bachelor.h"
#include "SpaceAndPlantSpawner.h"


// Sets default values
ASpaceAndPlantSpawner::ASpaceAndPlantSpawner()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	UWorld* const World = GetWorld();
	
}

// Called when the game starts or when spawned
void ASpaceAndPlantSpawner::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ASpaceAndPlantSpawner::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );

}

