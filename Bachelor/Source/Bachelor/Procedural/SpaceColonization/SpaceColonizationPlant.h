// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "Bachelor/Procedural/Data/TreeConstructionData.h"
#include "SpaceColonizationPlant.generated.h"

class UProceduralMeshComponent;
class AColonizationSpace;
struct FBranch;
struct FMeshData;

DECLARE_STATS_GROUP(TEXT("SpaceColonizationPlant"), STATGROUP_SpaceColonization, STATCAT_Advanced);

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

#if WITH_EDITOR  
	virtual void PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent);
#endif

	UFUNCTION(BlueprintCallable, Category = "Procedural")
		void GenerateTreeStructure();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Procedural|Visual")
		UProceduralMeshComponent* Mesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Procedural|Visual")
		FTreeConstructionData TreeConstructionData;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Procedural|Visual")
		bool SmoothOutBranchingAngles;

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

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Procedural|GrowthParameters")
		int MaxNumberOfNotDidNotGrowNums;

private:
	void InitUtilityValues();

	void ColonizeGivenSpaces();

	void InitialRootGrowth();

	void DoRootGrowthIterations(float MaxDistanceToCSpaceCenter, FVector CSpaceLocation);

	AColonizationSpace* GetNearestColonizationSpace();

	void GrowthIteration();

	bool CheckAllColonizationPoints();

	bool CheckColonizationPoint(FVector* ColonizationPoint);

	void RemoveFromGrowthSpaces(FVector ToRemove);

	void GrowAllBranches();

	void GrowBranch(FBranch* ToGrow);

	void TryCreatingNewBranch(FBranch* Parent, FVector NormalizedGrowthDirection, float IndividualGrowthPerIteration);

	FMeshData* AllMeshData;

	FBranch* RootBranch;

	TSet<FBranch*> GrowingBranches;

	FVector ActorLocation;

	float KillDistanceSquared;
	float RadiusOfInfluenceSquared;

	int CurrentColonizationPointCount;

	bool IsStillGrowing;

	bool TreeStructureWasGenerated;
};
