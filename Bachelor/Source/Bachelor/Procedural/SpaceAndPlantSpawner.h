// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "SpaceAndPlantSpawner.generated.h"

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
		int MaxNumberOfVerticesPerCylinderRing;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Procedural|ColonizationSpace")
		float ColonizationRadius;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Procedural|ColonizationSpace")
		float NumberOfGenerationPoints;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Procedural|ColonizationSpace")
		int RandomSeed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Procedural|ColonizationSpace|Debug")
		bool DrawDebugPoints;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Procedural|ColonizationSpace|Debug")
		FColor DebugPointColor;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Procedural|ColonizationSpace|Debug")
		float DebugPointSize;
	
};
