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

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Material)
		UMaterialInterface* GeneratedTreeMaterial;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Procedural)
		UProceduralMeshComponent* Mesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Procedural)
		TArray<AColonizationSpace* > GrowthSpaces;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Procedural)
		float KillDistance;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Procedural)
		float RadiusOfInfluence;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Procedural)
		float GrowthPerIteration;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Procedural)
		float MaxNumGrowthIterations;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Test)
		int MeshSectionToChange;

private:
	void InitUtilityValues();

	void ColonizeGivenSpaces();

	void InitialRootGrowth();

	AColonizationSpace* GetNearestColonizationSpace();

	void GrowthIteration();

	void CheckColonizationPoint(FVector ColonizationPoint);

	void RemoveFromGrowthSpaces(FVector ToRemove);

	void GrowBranch(FBranch* ToGrow);

	void GenerateTreeMesh();

	void GenerateBranchMesh(FBranch* Origin, TSet<FBranch*>& AllBranches, int MeshSection);

	TSet<FBranch*> RecursiveGetAllBranches(FBranch* Parent);

	FBranch* RootBranch;

	TSet<FBranch*> GrowingBranches;
	
	float KillDistanceSquared;
	float RadiusOfInfluenceSquared;
};
