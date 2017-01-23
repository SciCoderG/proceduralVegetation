// Fill out your copyright notice in the Description page of Project Settings.

#include "Bachelor.h"
#include "SpaceAndPlantSpawner.h"
#include "ColonizationSpaces/SphericalColonizationSpace.h"
#include "ColonizationSpaces/CylindricalColonizationSpace.h"
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
	SphericalRadius = 250.0f;

	CylinderHeight = 400.0f;

	CylinderRadius = 100.0f;

	ColonizationSpaceType = EColonizationSpaceType::VE_Spherical;

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

	Tropism = FVector(0.f, 0.f, 0.8f);

	TrunkRadiusMultiplier = 1.41f;

	MaxNumberOfBranchingTwigs = 5;

	MaxGrowthDepth = 5;

	WeightedGrowth = true;

	MinNumberOfSectionsPerBranch = 2;

	MaxNumberOfSectionsPerBranch = 8;

	BranchRadiusZero = 1.0f;

	BranchRadiusGrowthParameter = 2.0f;

	PolyReductionByCurveReduction = false;
	SmoothOutBranchingAngles = true;
}

// Called when the game starts or when spawned
void ASpaceAndPlantSpawner::BeginPlay()
{
	Super::BeginPlay();
	World = GetWorld();

	int wrapValue = FMath::Sqrt(NumberOfSpacesAndPlantsToSpawn);
	FVector xOffset = FVector(SphericalRadius * 2.0f, 0.f, 0.f);
	FVector yOffset = FVector(0.f, SphericalRadius * 2.0f, 0.f);
	for (int i = 0; i < NumberOfSpacesAndPlantsToSpawn; ++i) {
		FVector location = GetActorLocation();
		location += (i % wrapValue) * xOffset;
		location += (i / wrapValue) * yOffset;
		ASphericalColonizationSpace* colonizationSpace = SpawnSphericalColonizationSpace(location + ColonizationSpaceOffset);
		SpawnColonizationPlant(location, colonizationSpace);
	}
}

// Called every frame
void ASpaceAndPlantSpawner::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );
}

void ASpaceAndPlantSpawner::SetGeneralCSParameters(AColonizationSpace* ToSet) {
	ToSet->NumberOfGenerationPoints = NumberOfGenerationPoints;
	ToSet->RandomSeed = RandomSeedForAllSpaces;
	ToSet->DrawDebugPoints = DrawDebugPoints;
	ToSet->DebugPointColor = DebugPointColor;
	ToSet->DebugPointSize = DebugPointSize;
}


ASphericalColonizationSpace* ASpaceAndPlantSpawner::SpawnSphericalColonizationSpace(FVector Location) {
	ASphericalColonizationSpace* spawnedColonizationSpace = NULL;
	if (World) {
		UClass* colonizationSpaceClass = ASphericalColonizationSpace::StaticClass();
		spawnedColonizationSpace = World->SpawnActor<ASphericalColonizationSpace>(colonizationSpaceClass, Location, FRotator(0.f), FActorSpawnParameters());

		spawnedColonizationSpace->ColonizationRadius = SphericalRadius;
		SetGeneralCSParameters(spawnedColonizationSpace);
	}
	return spawnedColonizationSpace;
}

ACylindricalColonizationSpace* ASpaceAndPlantSpawner::SpawnCylindricalColonizationSpace(FVector Location) {
	ACylindricalColonizationSpace* spawnedColonizationSpace = NULL;
	if (World) {
		UClass* colonizationSpaceClass = ACylindricalColonizationSpace::StaticClass();
		spawnedColonizationSpace = World->SpawnActor<ACylindricalColonizationSpace>(colonizationSpaceClass, Location, FRotator(0.f), FActorSpawnParameters());

		spawnedColonizationSpace->CylinderHeight = CylinderHeight;
		spawnedColonizationSpace->CylinderRadius = CylinderRadius;
		SetGeneralCSParameters(spawnedColonizationSpace);
	}
	return spawnedColonizationSpace;
}

ASpaceColonizationPlant* ASpaceAndPlantSpawner::SpawnColonizationPlant(FVector Location, ASphericalColonizationSpace* ColonizationSpace) {
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
		spawnedColonizationPlant->Tropism = Tropism;
		spawnedColonizationPlant->TrunkRadiusMultiplier = TrunkRadiusMultiplier;
		spawnedColonizationPlant->MaxNumberOfBranchingTwigs = MaxNumberOfBranchingTwigs;
		spawnedColonizationPlant->MaxGrowthDepth = MaxGrowthDepth;
		spawnedColonizationPlant->WeightedGrowth = WeightedGrowth;
		spawnedColonizationPlant->MinNumberOfSectionsPerBranch = MinNumberOfSectionsPerBranch;
		spawnedColonizationPlant->MaxNumberOfSectionsPerBranch = MaxNumberOfSectionsPerBranch;
		spawnedColonizationPlant->BranchRadiusZero = BranchRadiusZero;
		spawnedColonizationPlant->BranchRadiusGrowthParameter = BranchRadiusGrowthParameter;
		spawnedColonizationPlant->PolyReductionByCurveReduction = PolyReductionByCurveReduction;
		spawnedColonizationPlant->SmoothOutBranchingAngles = SmoothOutBranchingAngles;
	}
	return spawnedColonizationPlant;
}



