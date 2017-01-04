// Fill out your copyright notice in the Description page of Project Settings.

#include "Bachelor.h"
#include "SpaceAndPlantSpawner.h"
#include "ColonizationSpace.h"
#include "SpaceColonizationPlant.h"

// Sets default values
ASpaceAndPlantSpawner::ASpaceAndPlantSpawner()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	World = GetWorld();

	// Spawning Parameters
	NumberOfSpacesAndPlantsToSpawn = 20;

	MaxNumberOfVerticesPerMeshSection = 400;

	ColonizationSpaceOffset = FVector(0.f, 0.f, 350.f);

	ShouldGenerateContinuousNumberOfGrowthIterations = false;

	// ColonizationSpace Parameters
	ColonizationRadius = 250.0f;

	NumberOfGenerationPoints = 2000;

	RandomSeedForAllSpaces = -1.f;

	DrawDebugPoints = false;

	DebugPointColor = FColor::Black;

	DebugPointSize = 5.f;

	// SpaceColonizationPlant Parameters
	KillDistance = 100.0f;

	RadiusOfInfluence = 500.0f;

	GrowthPerIteration = 20.0f;

	MaxNumGrowthIterations = 30;

	MaxNumberOfBranchingTwigs = 5;

	MaxGrowthDepth = 5;

	RootBranchRadius = 7.5f;

	MaxNumberOfSectionsPerBranch = 8;
}

// Called when the game starts or when spawned
void ASpaceAndPlantSpawner::BeginPlay()
{
	Super::BeginPlay();
	World = GetWorld();

	int wrapValue = FMath::Sqrt(NumberOfSpacesAndPlantsToSpawn);
	FVector xOffset = FVector(ColonizationRadius * 2.0f, 0.f, 0.f);
	FVector yOffset = FVector(0.f, ColonizationRadius * 2.0f, 0.f);
	for (int i = 0; i < NumberOfSpacesAndPlantsToSpawn; ++i) {
		FVector location = GetActorLocation();
		location += (i % wrapValue) * xOffset;
		location += (i / wrapValue) * yOffset;

		AColonizationSpace* colonizationSpace = SpawnColonizationSpace(location + ColonizationSpaceOffset);
		SpawnColonizationPlant(location, colonizationSpace);
	}
}

// Called every frame
void ASpaceAndPlantSpawner::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );
}


AColonizationSpace* ASpaceAndPlantSpawner::SpawnColonizationSpace(FVector Location) {
	AColonizationSpace* spawnedColonizationSpace = NULL;
	if (World) {
		UClass* colonizationSpaceClass = AColonizationSpace::StaticClass();
		spawnedColonizationSpace = World->SpawnActor<AColonizationSpace>(colonizationSpaceClass, Location, FRotator(0.f), FActorSpawnParameters());

		spawnedColonizationSpace->ColonizationRadius = ColonizationRadius;
		spawnedColonizationSpace->NumberOfGenerationPoints = NumberOfGenerationPoints;
		spawnedColonizationSpace->RandomSeed = RandomSeedForAllSpaces;
		spawnedColonizationSpace->DrawDebugPoints = DrawDebugPoints;
		spawnedColonizationSpace->DebugPointColor = DebugPointColor;
		spawnedColonizationSpace->DebugPointSize = DebugPointSize;
	}
	return spawnedColonizationSpace;
}

ASpaceColonizationPlant* ASpaceAndPlantSpawner::SpawnColonizationPlant(FVector Location, AColonizationSpace* ColonizationSpace) {
	ASpaceColonizationPlant* spawnedColonizationPlant = NULL;
	if (World) {
		UClass* colonizationPlantClass = ASpaceColonizationPlant::StaticClass();
		spawnedColonizationPlant = World->SpawnActor<ASpaceColonizationPlant>(colonizationPlantClass, Location, FRotator(0.f), FActorSpawnParameters());

		spawnedColonizationPlant->GrowthSpaces.Add(ColonizationSpace);
		spawnedColonizationPlant->MaxNumberOfVerticesPerMeshSection = MaxNumberOfVerticesPerMeshSection;
		spawnedColonizationPlant->KillDistance = KillDistance;
		spawnedColonizationPlant->RadiusOfInfluence = RadiusOfInfluence;
		spawnedColonizationPlant->GrowthPerIteration = GrowthPerIteration;
		spawnedColonizationPlant->MaxNumGrowthIterations = MaxNumGrowthIterations;
		spawnedColonizationPlant->MaxNumberOfBranchingTwigs = MaxNumberOfBranchingTwigs;
		spawnedColonizationPlant->MaxGrowthDepth = MaxGrowthDepth;
		spawnedColonizationPlant->RootBranchRadius = RootBranchRadius;
		spawnedColonizationPlant->MaxNumberOfSectionsPerBranch = MaxNumberOfSectionsPerBranch;
	}
	return spawnedColonizationPlant;
}

