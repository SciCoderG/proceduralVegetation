#pragma once

#include "UnrealMath.h"
#include "Procedural/Data/MeshData.h"
#include "Branch.generated.h"

USTRUCT()
struct BACHELOR_API FBranch
{
	GENERATED_USTRUCT_BODY()
public:
	FBranch() {
		ParentBranch = NULL;

		Start = FVector(0);
		End = FVector(0);
		StartRadius = 0.f;
		EndRadius = 0.f;

		StartConnectionNormal = FVector(0);
		EndConnectionNormal = FVector(0);

		GrowDirection = FVector(0);

		GrowCount = 0;
		BranchDepth = 0;
	}

	void ResetForNextGrowthIteration();

	FBranch* ParentBranch;
	TArray<FBranch*> ChildBranches;
	int BranchDepth;

	FVector Start;
	FVector End;
	float StartRadius;
	float EndRadius;

	FVector StartConnectionNormal;
	FVector EndConnectionNormal;

	FVector GrowDirection;
	int GrowCount;
};

