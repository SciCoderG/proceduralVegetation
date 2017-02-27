// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "Bachelor/Procedural/Data/TreeConstructionData.h"

#include "SpaceAndPlantSpawner.generated.h"

class ASpaceColonizationPlant;
class AColonizationSpace;
class ASphericalColonizationSpace;
class ACylindricalColonizationSpace;

UENUM(BlueprintType)	
enum class EColonizationSpaceType : uint8
{
	VE_Spherical	UMETA(DisplayName = "Spherical"),
	VE_Cylindrical 	UMETA(DisplayName = "Cylindrical")
};

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
		FVector ColonizationSpaceOffset;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Procedural|Plant|GrowthParameters")
		float KillDistance;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Procedural|Plant|GrowthParameters")
		float RadiusOfInfluence;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Procedural|Plant|GrowthParameters")
		float GrowthPerIteration;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Procedural|Plant|GrowthParameters")
		int MaxNumGrowthIterations;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Procedural|Plant|GrowthParameters")
		FVector Tropism;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Procedural|Plant|GrowthParameters")
		int MaxNumberOfBranchingTwigs;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Procedural|Plant|GrowthParameters")
		int MaxGrowthDepth;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Procedural|Plant|GrowthParameters")
		bool WeightedGrowth;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Procedural|Plant|GrowthParameters")
		int MaxNumberOfNotDidNotGrowNums;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Procedural|Plant|Visual")
		FTreeConstructionData TreeConstructionData;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Procedural|Plant|Visual")
		bool SmoothOutBranchingAngles;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Procedural|ColonizationSpace|Type")
		EColonizationSpaceType ColonizationSpaceType;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Procedural|ColonizationSpace|Type")
		float SphericalRadius;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Procedural|ColonizationSpace|Type")
		float CylinderHeight;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Procedural|ColonizationSpace|Type")
		float CylinderRadius;
	

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

	void SetGeneralCSParameters(AColonizationSpace* ToSet);

	ASphericalColonizationSpace* SpawnSphericalColonizationSpace(FVector Location);

	ACylindricalColonizationSpace* SpawnCylindricalColonizationSpace(FVector Location);

	ASpaceColonizationPlant* SpawnColonizationPlant(FVector Location, AColonizationSpace* ColonizationSpace);

	UWorld* World;
	
};
