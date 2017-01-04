// Fill out your copyright notice in the Description page of Project Settings.

#include "Bachelor.h"
#include "MeshConstructor.h"
#include "Procedural/Data/MeshData.h"
#include "Procedural/Branch.h"
#include "MeshDataConstructor.h"

#include "ProceduralMeshComponent.h"
#include "KismetProceduralMeshLibrary.h"


void UMeshConstructor::GenerateTreeMesh(UProceduralMeshComponent* Mesh, FMeshData& AllMeshData, 
	FBranch* RootBranch, int MaxNumberOfSectionsPerBranch, int MaxNumberOfVerticesPerMeshSection) {
	if (Mesh->GetNumSections() > 1) {
		Mesh->ClearAllMeshSections();
	}

	TSet<FBranch*> allBranches = RecursiveGetAllBranches(RootBranch);

	AllMeshData.Reset();
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

TArray<FBranch*> RecursiveGetAllBranchesOnSameDepth(FBranch* Parent) {
	TArray<FBranch*> BranchesOnSameDepth;
	for (FBranch* childBranch : Parent->ChildBranches) {
		if (childBranch->BranchDepth == Parent->BranchDepth) {
			BranchesOnSameDepth.Add(childBranch);
			BranchesOnSameDepth.Append(RecursiveGetAllBranchesOnSameDepth(childBranch));
		}
	}
	return BranchesOnSameDepth;
}

void UMeshConstructor::GenerateBranchMesh(FMeshData& AllMeshData, FBranch* Origin, TSet<FBranch*>& AllBranches, int MeshSection, 
	int NumberOfSectionsPerBranch) {
	TArray<FBranch*> BranchesOnSameDepth = RecursiveGetAllBranchesOnSameDepth(Origin);
	for (FBranch* currentBranch : BranchesOnSameDepth) {
		AllBranches.Remove(currentBranch);
	}
	float childrenAdjustment = 0.0f;
	if (NULL != Origin->ParentBranch) {
		childrenAdjustment = Origin->ParentBranch->ChildBranches.Num();
	}

	// TODO - get Radii from branches
	float ringRadius = 5.0f / (Origin->BranchDepth + 1 + childrenAdjustment);

	TArray<FVector> RingCenters;
	RingCenters.Add(Origin->Start);
	RingCenters.Add(Origin->End);

	TArray<float> RingRadii;
	RingRadii.Add(ringRadius);
	RingRadii.Add(ringRadius);
	for (FBranch* currentBranch : BranchesOnSameDepth) {
		RingCenters.Add(currentBranch->End);
		RingRadii.Add(ringRadius);
	}

	UMeshDataConstructor::GenerateMultiLevelCylinder(AllMeshData, RingCenters, RingRadii, NumberOfSectionsPerBranch);
}

TSet<FBranch*> UMeshConstructor::RecursiveGetAllBranches(FBranch* Parent) {
	TSet<FBranch*> parentAndChildren;
	parentAndChildren.Add(Parent);
	for (FBranch* childBranch : Parent->ChildBranches) {
		parentAndChildren.Append(RecursiveGetAllBranches(childBranch));
	}
	return parentAndChildren;
}

