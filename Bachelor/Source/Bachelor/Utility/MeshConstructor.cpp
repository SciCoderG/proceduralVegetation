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
	FBranch* RootBranch, float TrunkRadiusMultiplier, int MinNumberOfSectionsPerBranch, int MaxNumberOfSectionsPerBranch, int MaxNumberOfVerticesPerMeshSection,
	float BranchRadiusZero, float BranchRadiusGrowthParameter) {
	if (Mesh->GetNumSections() > 1) {
		Mesh->ClearAllMeshSections();
	}
	AllMeshData.Reset();

	TArray<FBranch*> allBranches = UBranchUtility::RecursiveGetAllBranchesAsArray(RootBranch);
	UE_LOG(LogTemp, Warning, TEXT("Number of Branches: %d"), allBranches.Num());
	UBranchUtility::RecursiveCalculateAllBranchRadii(RootBranch, BranchRadiusZero, BranchRadiusGrowthParameter);
	RootBranch->StartRadius *= TrunkRadiusMultiplier;
	float RootMaxRadius = RootBranch->StartRadius;

	UBranchUtility::CalcAllBranchConnectionNormals(RootBranch);
	UBranchUtility::CalcPerBranchDepthZRotAngle(RootBranch, PI / 1.5f);

	int vertexCounter = 0;
	int meshSectionCount = 0;
	while(allBranches.Num() > 0){
		FBranch* currentBranch = allBranches[0];
		GenerateBranchMesh(AllMeshData, currentBranch, allBranches, RootMaxRadius, MinNumberOfSectionsPerBranch, MaxNumberOfSectionsPerBranch);

		if (AllMeshData.Vertices.Num() > MaxNumberOfVerticesPerMeshSection) {
			Mesh->CreateMeshSection(meshSectionCount, AllMeshData.Vertices, AllMeshData.Triangles, AllMeshData.Normals,
				AllMeshData.UVs, TArray<FColor>(), AllMeshData.Tangents, false);
			meshSectionCount++;
			vertexCounter += AllMeshData.Vertices.Num();
			AllMeshData.Reset();
		}
	}
	Mesh->CreateMeshSection(meshSectionCount, AllMeshData.Vertices, AllMeshData.Triangles, AllMeshData.Normals,
		AllMeshData.UVs, TArray<FColor>(), AllMeshData.Tangents, false);
	meshSectionCount++;
	vertexCounter += AllMeshData.Vertices.Num();
	AllMeshData.Reset();

	UE_LOG(LogTemp, Warning, TEXT("Generated %d Vertices"), vertexCounter);
	UE_LOG(LogTemp, Warning, TEXT("Generated %d MeshSections"), Mesh->GetNumSections());
}

void UMeshConstructor::GenerateBranchMesh(FMeshData& AllMeshData, FBranch* Origin, TArray<FBranch*>& AllBranches, float RootRadius,
	int MinNumberOfSectionsPerBranch, int MaxNumberOfSectionsPerBranch) {
	TArray<FBranch*> BranchesOnSameDepth = UBranchUtility::RecursiveGetAllBranchesOnSameDepth(Origin);
	for (FBranch* currentBranch : BranchesOnSameDepth) {
		AllBranches.Remove(currentBranch);
	}
	//UE_LOG(LogTemp, Warning, TEXT("Num Of Branches as one cylinder: %d"), BranchesOnSameDepth.Num());
	TArray<FVector> RingCenters;
	RingCenters.Add(Origin->Start);

	TArray<FVector> ConnectionNormals;
	ConnectionNormals.Add(Origin->StartConnectionNormal);

	TArray<float> RingRadii;
	RingRadii.Add(Origin->StartRadius);
	
	for (FBranch* currentBranch : BranchesOnSameDepth) {
		RingCenters.Add(currentBranch->End);
		RingRadii.Add(currentBranch->EndRadius);
		ConnectionNormals.Add(currentBranch->EndConnectionNormal);
	}

	int NumberOfSectionsUsed = UBranchUtility::CalcNumOfBranchSections(RingRadii[0], RootRadius, MinNumberOfSectionsPerBranch, MaxNumberOfSectionsPerBranch);

	UMeshDataConstructor::GenerateMultiLevelCylinder(AllMeshData, RingCenters, ConnectionNormals, RingRadii, NumberOfSectionsUsed, Origin->ZRotationAngle);
}