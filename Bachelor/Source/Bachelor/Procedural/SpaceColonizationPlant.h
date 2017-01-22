// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Data/MeshData.h"
#include "GameFramework/Actor.h"
#include "SpaceColonizationPlant.generated.h"

class UProceduralMeshComponent;
class AColonizationSpace;
struct FBranch;

UCLASS()
class BACHELOR_API ASpaceColonizationPlant : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASpaceColonizationPlant();

	virtual ~ASpaceColonizationPlant();

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	// Called every frame
	virtual void Tick( float DeltaSeconds ) override;

	virtual void PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Procdeural|Visual")
		UMaterialInterface* GeneratedTreeMaterial;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Procedural|Visual")
		UProceduralMeshComponent* Mesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Procedural|Visual")
		int MinNumberOfSectionsPerBranch;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Procedural|Visual")
		int MaxNumberOfSectionsPerBranch;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Procedural|Visual")
		int MaxNumberOfVerticesPerMeshSection;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Procedural|Visual")
		float BranchRadiusZero;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Procedural|Visual")
		float BranchRadiusGrowthParameter;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Procedural|Visual")
		bool SmoothOutBranchingAngles;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Procedural|Visual")
		bool PolyReductionByCurveReduction;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Procedural|GrowthSpaces")
		TArray<AColonizationSpace* > GrowthSpaces;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Procedural|GrowthParameters")
		float KillDistance;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Procedural|GrowthParameters")
		float RadiusOfInfluence;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Procedural|GrowthParameters")
		float GrowthPerIteration;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Procedural|GrowthParameters")
		FVector Tropism;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Procedural|GrowthParameters")
		int MaxNumGrowthIterations;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Procedural|GrowthParameters")
		int MaxNumberOfBranchingTwigs;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Procedural|GrowthParameters")
		int MaxGrowthDepth;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Procedural|GrowthParameters")
		bool WeightedGrowth;

private:
	void InitUtilityValues();

	void ColonizeGivenSpaces();

	void InitialRootGrowth();

	AColonizationSpace* GetNearestColonizationSpace();

	void GrowthIteration();

	TSet<FVector>& GetAllColonizationPoints();

	bool CheckAllColonizationPoints();

	void CheckIfInKillZone(FVector ColonizationPoint);

	bool CheckColonizationPoint(FVector ColonizationPoint);

	void RemoveFromGrowthSpaces(FVector ToRemove);

	void GrowAllBranches();

	void GrowBranch(FBranch* ToGrow);

	void TryCreatingNewBranch(FBranch* Parent, FVector NormalizedGrowthDirection, float IndividualGrowthPerIteration);

	FMeshData AllMeshData;

	FBranch* RootBranch;

	TSet<FBranch*> GrowingBranches;
	TSet<FVector> AllColonizationPoints;

	float KillDistanceSquared;
	float RadiusOfInfluenceSquared;

	bool IsStillGrowing;
};
