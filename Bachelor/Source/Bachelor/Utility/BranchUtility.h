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

	static TSet<FBranch*> RecursiveGetAllLeaves(FBranch* RootBranch);

	static float RecursiveCalculateAllBranchRadii(FBranch* Parent, float RadiusZero, float BranchingRadiusParameter);

	static void CalcAllBranchConnectionNormals(FBranch* Current);

	static int CalcNumOfBranchSections(float MaxRadius, int MinNumberOfSectionsPerBranch, int MaxNumberOfSectionsPerBranch);

	static void RecursiveReduceGrownBranches(FBranch* Parent);
	
	static void RecursiveDeleteAllBranches(FBranch* Parent);

	static void SmoothOutBranchingAngles(FBranch* Current);

private:
	static void ElongateGrownBranches(FBranch* Parent);

};
