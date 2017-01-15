// Fill out your copyright notice in the Description page of Project Settings.

#include "Bachelor.h"
#include "MeshConstructor.h"
#include "Procedural/Data/MeshData.h"
#include "Procedural/Branch.h"
#include "MeshDataConstructor.h"
#include "BranchUtility.h"

#include "ProceduralMeshComponent.h"
#include "KismetProceduralMeshLibrary.h"


void UMeshConstructor::GenerateTreeMesh(UProceduralMeshComponent* Mesh, FMeshData& AllMeshData, 
	FBranch* RootBranch, int MaxNumberOfSectionsPerBranch, int MaxNumberOfVerticesPerMeshSection, float BranchRadiusZero, float BranchRadiusGrowthParameter) {
	if (Mesh->GetNumSections() > 1) {
		Mesh->ClearAllMeshSections();
	}

	TSet<FBranch*> allBranches = UBranchUtility::RecursiveGetAllBranches(RootBranch);

	AllMeshData.Reset();

	UBranchUtility::RecursiveCalculateAllBranchRadii(RootBranch, BranchRadiusZero, BranchRadiusGrowthParameter);

	int meshSectionCount = 0;
	int i = 0;
	for (FBranch* currentBranch : allBranches) {
		GenerateBranchMesh(AllMeshData, currentBranch, allBranches, i, MaxNumberOfSectionsPerBranch);

		if (AllMeshData.Vertices.Num() > MaxNumberOfVerticesPerMeshSection) {
			Mesh->CreateMeshSection(meshSectionCount, AllMeshData.Vertices, AllMeshData.Triangles, AllMeshData.Normals,
				AllMeshData.UVs, TArray<FColor>(), AllMeshData.Tangents, false);
			meshSectionCount++;
			AllMeshData.Reset();
		}
		++i;
	}
	UE_LOG(LogTemp, Warning, TEXT("Generated %d MeshSections"), Mesh->GetNumSections());
}




void UMeshConstructor::GenerateBranchMesh(FMeshData& AllMeshData, FBranch* Origin, TSet<FBranch*>& AllBranches, int MeshSection, 
	int NumberOfSectionsPerBranch) {
	TArray<FBranch*> BranchesOnSameDepth = UBranchUtility::RecursiveGetAllBranchesOnSameDepth(Origin);
	for (FBranch* currentBranch : BranchesOnSameDepth) {
		AllBranches.Remove(currentBranch);
	}
	float childrenAdjustment = 0.0f;
	if (NULL != Origin->ParentBranch) {
		childrenAdjustment = Origin->ParentBranch->ChildBranches.Num();
	}

	TArray<FVector> RingCenters;
	RingCenters.Add(Origin->Start);
	RingCenters.Add(Origin->End);

	TArray<float> RingRadii;
	RingRadii.Add(Origin->StartRadius);
	RingRadii.Add(Origin->EndRadius);
	for (FBranch* currentBranch : BranchesOnSameDepth) {
		RingCenters.Add(currentBranch->End);
		RingRadii.Add(currentBranch->EndRadius);
	}

	UMeshDataConstructor::GenerateMultiLevelCylinder(AllMeshData, RingCenters, RingRadii, NumberOfSectionsPerBranch);
}



