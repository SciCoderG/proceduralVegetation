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
	FBranch* RootBranch, int MinNumberOfSectionsPerBranch, int MaxNumberOfSectionsPerBranch, int MaxNumberOfVerticesPerMeshSection,
	float BranchRadiusZero, float BranchRadiusGrowthParameter) {
	if (Mesh->GetNumSections() > 1) {
		Mesh->ClearAllMeshSections();
	}

	TArray<FBranch*> allBranches = UBranchUtility::RecursiveGetAllBranchesAsArray(RootBranch);

	UE_LOG(LogTemp, Warning, TEXT("Number of Branches: %d"), allBranches.Num());
	AllMeshData.Reset();

	UBranchUtility::RecursiveCalculateAllBranchRadii(RootBranch, BranchRadiusZero, BranchRadiusGrowthParameter);
	int vertexCounter = 0;
	int meshSectionCount = 0;
	int i = 0;
	while(allBranches.Num() > 0){
		FBranch* currentBranch = allBranches[0];
		GenerateBranchMesh(AllMeshData, currentBranch, allBranches, MinNumberOfSectionsPerBranch, MaxNumberOfSectionsPerBranch);

		if (AllMeshData.Vertices.Num() > MaxNumberOfVerticesPerMeshSection) {
			Mesh->CreateMeshSection(meshSectionCount, AllMeshData.Vertices, AllMeshData.Triangles, AllMeshData.Normals,
				AllMeshData.UVs, TArray<FColor>(), AllMeshData.Tangents, false);
			meshSectionCount++;
			vertexCounter += AllMeshData.Vertices.Num();
			AllMeshData.Reset();
		}
		++i;
	}
	Mesh->CreateMeshSection(meshSectionCount, AllMeshData.Vertices, AllMeshData.Triangles, AllMeshData.Normals,
		AllMeshData.UVs, TArray<FColor>(), AllMeshData.Tangents, false);
	meshSectionCount++;
	vertexCounter += AllMeshData.Vertices.Num();
	AllMeshData.Reset();

	//UE_LOG(LogTemp, Warning, TEXT("Called \"GenerateBranchMesh\" %d Times"), i);
	UE_LOG(LogTemp, Warning, TEXT("Generated %d Vertices"), vertexCounter);
	UE_LOG(LogTemp, Warning, TEXT("Generated %d MeshSections"), Mesh->GetNumSections());
}


int CalculatedNumberOfSections(float MaxRadius, int MinNumberOfSectionsPerBranch, int MaxNumberOfSectionsPerBranch) {
	if (MaxNumberOfSectionsPerBranch < 2) {
		UE_LOG(LogTemp, Warning, TEXT("Max Number of Sections Per Branch: %d - Should be equal to or greater than 2."), MaxNumberOfSectionsPerBranch);
		MaxNumberOfSectionsPerBranch = 2;
	}

	int NumberOfSectionsUsed = MaxNumberOfSectionsPerBranch;
	int CalculatedNumberOfSections = ((MaxRadius / 2.0f) - 1) + MinNumberOfSectionsPerBranch;
	if (CalculatedNumberOfSections < MaxNumberOfSectionsPerBranch) {
		NumberOfSectionsUsed = CalculatedNumberOfSections;
	}
	return NumberOfSectionsUsed;
}

void UMeshConstructor::GenerateBranchMesh(FMeshData& AllMeshData, FBranch* Origin, TArray<FBranch*>& AllBranches,
	int MinNumberOfSectionsPerBranch, int MaxNumberOfSectionsPerBranch) {
	TArray<FBranch*> BranchesOnSameDepth = UBranchUtility::RecursiveGetAllBranchesOnSameDepth(Origin);
	for (FBranch* currentBranch : BranchesOnSameDepth) {
		AllBranches.Remove(currentBranch);
	}
	//UE_LOG(LogTemp, Warning, TEXT("Num Of Branches as one cylinder: %d"), BranchesOnSameDepth.Num());
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

	int NumberOfSectionsUsed = CalculatedNumberOfSections(RingRadii[0], MinNumberOfSectionsPerBranch, MaxNumberOfSectionsPerBranch);
	
	UMeshDataConstructor::GenerateMultiLevelCylinder(AllMeshData, RingCenters, RingRadii, NumberOfSectionsUsed);
}



