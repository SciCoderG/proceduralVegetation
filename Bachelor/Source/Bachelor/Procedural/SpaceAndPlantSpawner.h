// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "SpaceAndPlantSpawner.generated.h"

class ASpaceColonizationPlant;
class AColonizationSpace;

UCLASS()
class BACHELOR_API ASpaceAndPlantSpawner : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASpaceAndPlantSpawner();

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	// Called every frame
	virtual void Tick( float DeltaSeconds ) override;

	//UFUNCTION(BlueprintCallable, Category = "Procedural|Plant")

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Procedural|SpawningParameters")
		int NumberOfSpacesAndPlantsToSpawn;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Procedural|SpawningParameters")
		int MaxNumberOfVerticesPerMeshSection;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Procedural|SpawningParameters")
		FVector ColonizationSpaceOffset;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Procedural|SpawningParameters")
		bool ShouldGenerateContinuousNumberOfGrowthIterations;


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Procedural|Plant|GrowthParameters")
		float KillDistance;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Procedural|Plant|GrowthParameters")
		float RadiusOfInfluence;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Procedural|Plant|GrowthParameters")
		float GrowthPerIteration;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Procedural|Plant|GrowthParameters")
		int MaxNumGrowthIterations;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Procedural|Plant|GrowthParameters")
		int MaxNumberOfBranchingTwigs;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Procedural|Plant|GrowthParameters")
		int MaxGrowthDepth;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Procedural|Plant|GrowthParameters")
		float RootBranchRadius;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Procedural|Plant|Visual")
		int MaxNumberOfSectionsPerBranch;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Procedural|Plant|Visual")
		float BranchRadiusZero;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Procedural|Plant|Visual")
		float BranchRadiusGrowthParameter;


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Procedural|ColonizationSpace")
		float ColonizationRadius;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Procedural|ColonizationSpace")
		float NumberOfGenerationPoints;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Procedural|ColonizationSpace")
		int RandomSeedForAllSpaces;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Procedural|ColonizationSpace|Debug")
		bool DrawDebugPoints;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Procedural|ColonizationSpace|Debug")
		FColor DebugPointColor;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Procedural|ColonizationSpace|Debug")
		float DebugPointSize;

private:

	AColonizationSpace* SpawnColonizationSpace(FVector Location);

	ASpaceColonizationPlant* SpawnColonizationPlant(FVector Location, AColonizationSpace* ColonizationSpace);

	UWorld* World;
	
};
