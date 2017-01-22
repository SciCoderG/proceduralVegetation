// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Kismet/BlueprintFunctionLibrary.h"
#include "MeshConstructor.generated.h"

class UProceduralMeshComponent;
struct FMeshData;
struct FBranch;

UCLASS()
class BACHELOR_API UMeshConstructor : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
	
public:
	static void GenerateTreeMesh(UProceduralMeshComponent* Mesh, FMeshData& AllMeshData, 
		FBranch* RootBranch, float TrunkRadiusMultiplier, int MinNumberOfSectionsPerBranch, int MaxNumberOfSectionsPerBranch, int MaxNumberOfVerticesPerMeshSection, float BranchRadiusZero, float BranchRadiusGrowthParameter);

	static void GenerateBranchMesh(FMeshData& AllMeshData, FBranch* Origin,
		TArray<FBranch*>& AllBranches, float RootRadius, int MinNumberOfSectionsPerBranch, int MaxNumberOfSectionsPerBranch);
	
};
