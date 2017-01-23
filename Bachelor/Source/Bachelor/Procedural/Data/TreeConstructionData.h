#pragma once

#include "UnrealMath.h"
#include "TreeConstructionData.generated.h"


class UProceduralMeshComponent;
struct FMeshData;
struct FBranch;

USTRUCT()
struct BACHELOR_API FTreeConstructionData
{
	GENERATED_USTRUCT_BODY()
public:

	FTreeConstructionData() {
		Mesh = NULL;
		AllMeshData = NULL;
		RootBranch = NULL;

		TrunkRadiusMultiplier = 1.41f;
		MinNumberOfSectionsPerBranch = 2;
		MaxNumberOfSectionsPerBranch = 6;
		MaxNumberOfVerticesPerMeshSection = 400;
		BranchRadiusZero = 2.0f;
		BranchRadiusGrowthParameter = 2.5f;
	}

	UProceduralMeshComponent* Mesh;
	FMeshData* AllMeshData;
	FBranch* RootBranch;
	
	float TrunkRadiusMultiplier;
	int MinNumberOfSectionsPerBranch;
	int MaxNumberOfSectionsPerBranch;
	int MaxNumberOfVerticesPerMeshSection;
	float BranchRadiusZero;
	float BranchRadiusGrowthParameter;
};

