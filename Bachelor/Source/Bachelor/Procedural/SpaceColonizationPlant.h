// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

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
		int MaxNumberOfVerticesPerCylinderRing;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Procedural|GrowthSpaces")
		TArray<AColonizationSpace* > GrowthSpaces;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Procedural|GrowthParameters")
		float KillDistance;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Procedural|GrowthParameters")
		float RadiusOfInfluence;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Procedural|GrowthParameters")
		float GrowthPerIteration;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Procedural|GrowthParameters")
		int MaxNumGrowthIterations;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Procedural|GrowthParameters")
		int MaxNumberOfBranchingTwigs;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Procedural|GrowthParameters")
		int MaxGrowthDepth;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Procedural|GrowthParameters")
		float RootBranchRadius;

private:
	void InitUtilityValues();

	void ColonizeGivenSpaces();

	void InitialRootGrowth();

	AColonizationSpace* GetNearestColonizationSpace();

	void GrowthIteration();

	TSet<FVector>& GetAllColonizationPoints();

	void CheckAllColonizationPoints();

	void CheckColonizationPoint(FVector ColonizationPoint);

	void RemoveFromGrowthSpaces(FVector ToRemove);

	void GrowAllBranches();

	void GrowBranch(FBranch* ToGrow);

	void GenerateTreeMesh();

	void GenerateBranchMesh(FBranch* Origin, TSet<FBranch*>& AllBranches, int MeshSection, int NumberOfVerticesPerCylinderRing);

	TSet<FBranch*> RecursiveGetAllBranches(FBranch* Parent);



	FBranch* RootBranch;

	TSet<FBranch*> GrowingBranches;
	TSet<FVector> AllColonizationPoints;

	float KillDistanceSquared;
	float RadiusOfInfluenceSquared;
};
