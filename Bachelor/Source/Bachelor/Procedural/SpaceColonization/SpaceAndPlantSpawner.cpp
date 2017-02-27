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

	ColonizationSpaceOffset = FVector(0.f, 0.f, 350.f);

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

	MaxNumberOfBranchingTwigs = 5;

	MaxGrowthDepth = 5;

	WeightedGrowth = true;

	SmoothOutBranchingAngles = true;

	MaxNumberOfNotDidNotGrowNums = 2;
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

		AColonizationSpace* colonizationSpace = NULL;
		FVector colonizationSpaceSpawnLocation = location + ColonizationSpaceOffset;

		switch (ColonizationSpaceType) {
		case EColonizationSpaceType::VE_Cylindrical:
			colonizationSpace = SpawnCylindricalColonizationSpace(colonizationSpaceSpawnLocation);
			break;
		default:
			colonizationSpace = SpawnSphericalColonizationSpace(colonizationSpaceSpawnLocation);
		}
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

	ToSet->GenerateRandomColonizationPoints();
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

int spawnCounter = 0;

ASpaceColonizationPlant* ASpaceAndPlantSpawner::SpawnColonizationPlant(FVector Location, AColonizationSpace* ColonizationSpace) {
	ASpaceColonizationPlant* spawnedColonizationPlant = NULL;
	if (World) {
		UClass* colonizationPlantClass = ASpaceColonizationPlant::StaticClass();
		
		FString currentName = this->GetName();
		currentName.AppendInt(spawnCounter);
		spawnCounter++;

		FActorSpawnParameters spawnParams = FActorSpawnParameters();
		spawnParams.Name = FName(*currentName);

		spawnedColonizationPlant = World->SpawnActor<ASpaceColonizationPlant>(colonizationPlantClass, Location, GetActorRotation(), spawnParams);

		spawnedColonizationPlant->GrowthSpaces.Add(ColonizationSpace);
		spawnedColonizationPlant->TreeConstructionData = TreeConstructionData;
		spawnedColonizationPlant->KillDistance = KillDistance;
		spawnedColonizationPlant->RadiusOfInfluence = RadiusOfInfluence;
		spawnedColonizationPlant->GrowthPerIteration = GrowthPerIteration;
		spawnedColonizationPlant->MaxNumGrowthIterations = MaxNumGrowthIterations;
		spawnedColonizationPlant->Tropism = Tropism;
		spawnedColonizationPlant->MaxNumberOfBranchingTwigs = MaxNumberOfBranchingTwigs;
		spawnedColonizationPlant->MaxGrowthDepth = MaxGrowthDepth;
		spawnedColonizationPlant->WeightedGrowth = WeightedGrowth;
		
		spawnedColonizationPlant->SmoothOutBranchingAngles = SmoothOutBranchingAngles;
		spawnedColonizationPlant->MaxNumberOfNotDidNotGrowNums = MaxNumberOfNotDidNotGrowNums;

		spawnedColonizationPlant->GenerateTreeStructure();
	}
	return spawnedColonizationPlant;
}



