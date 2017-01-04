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
		FBranch* RootBranch, int MaxNumberOfSectionsPerBranch, int MaxNumberOfVerticesPerMeshSection);
	
	static void GenerateBranchMesh(FMeshData& AllMeshData, FBranch* Origin,
		TSet<FBranch*>& AllBranches, int MeshSection, int NumberOfSectionsPerBranch);
	
	static TSet<FBranch*> RecursiveGetAllBranches(FBranch* Parent);

};
