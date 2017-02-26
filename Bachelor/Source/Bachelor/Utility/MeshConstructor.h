// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Kismet/BlueprintFunctionLibrary.h"
#include "MeshConstructor.generated.h"

class UProceduralMeshComponent;
struct FMeshData;
struct FBranch;
struct FTreeConstructionData;

DECLARE_STATS_GROUP(TEXT("UMeshConstructor"), STATGROUP_UMeshConstructor, STATCAT_Advanced);

UCLASS()
class BACHELOR_API UMeshConstructor : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
	
public:

	static void GenerateTreeMesh(FTreeConstructionData* TreeConstructionData);

	static void GenerateBranchMesh(FMeshData* AllMeshData, FBranch* Origin,
		TArray<FBranch*>& AllBranches, float RootRadius, int MinNumberOfSectionsPerBranch, int MaxNumberOfSectionsPerBranch);

	static void GenerateFractalMesh(FTreeConstructionData* TreeConstructionData, const TArray<FBranch*>& AllBranches);
};
