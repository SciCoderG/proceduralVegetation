// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Kismet/BlueprintFunctionLibrary.h"
#include "BranchUtility.generated.h"

struct FBranch;

UCLASS()
class BACHELOR_API UBranchUtility : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
	
public:

	static TSet<FBranch*> RecursiveGetAllBranches(FBranch* Parent);

	static TArray<FBranch*> RecursiveGetAllBranchesAsArray(FBranch* Parent);

	static TArray<FBranch*> RecursiveGetAllBranchesOnSameDepth(FBranch* Parent);

	static TArray<FBranch*> RecursiveGetAllLeaves(FBranch* RootBranch);

	static float RecursiveCalculateAllBranchRadii(FBranch* Parent, float RadiusZero, float BranchingRadiusParameter);

	static void CalcConnectionNormals(FBranch* Current);

	static void CalcTreeLikeConnectionNormals(FBranch* Current);

	static int CalcNumOfBranchSections(float CurrentRadius, float MaxRadius, int MinNumberOfSectionsPerBranch, int MaxNumberOfSectionsPerBranch);

	static void CalcPerBranchDepthZRotAngle(FBranch* Current, float RotationAngleIncrement);

	static void ReduceGrownBranchesByMaxDotProduct(FBranch* Parent, float MaxDotProduct);

	static void RecursiveReduceGrownBranches(FBranch* Parent);
	
	static void RecursiveDeleteAllBranches(FBranch* Parent);

	static void SmoothOutBranchingAngles(FBranch* Current);

private:
	static void ElongateGrownBranches(FBranch* Parent);

	static void MergeParentAndChild(FBranch* Parent, FBranch* Child);
};
