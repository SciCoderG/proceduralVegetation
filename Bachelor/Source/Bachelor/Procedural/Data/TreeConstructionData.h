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
		TreeMaterial = NULL;

		TrunkRadiusMultiplier = 1.41f;
		MinNumberOfSectionsPerBranch = 3;
		MaxNumberOfSectionsPerBranch = 4;
		MaxNumberOfVerticesPerMeshSection = 3e5;
		BranchRadiusZero = 3.0f;
		BranchRadiusGrowthParameter = 2.5f;
		DoBranchRadiusCalculations = true;
		ZRotationAnglePerBranch = 120;
		MakeFractalMesh = false;
		MaxDotProductBetweenParentAndChild = .95f;
	}

	UProceduralMeshComponent* Mesh;
	FMeshData* AllMeshData;
	FBranch* RootBranch;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Growth")
		bool DoBranchRadiusCalculations;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Growth")
		float TrunkRadiusMultiplier;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Growth")
		float BranchRadiusZero;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Growth")
		float BranchRadiusGrowthParameter;
	
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Visual")
		UMaterialInterface* TreeMaterial;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Visual")
		float ZRotationAnglePerBranch;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Visual")
		int MinNumberOfSectionsPerBranch;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Visual")
		int MaxNumberOfSectionsPerBranch;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Visual")
		int MaxNumberOfVerticesPerMeshSection;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Visual")
		float MaxDotProductBetweenParentAndChild;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Visual")
		bool MakeFractalMesh;
};

