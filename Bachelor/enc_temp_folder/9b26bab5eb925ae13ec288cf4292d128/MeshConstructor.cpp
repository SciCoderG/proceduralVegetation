// Fill out your copyright notice in the Description page of Project Settings.

#include "Bachelor.h"
#include "MeshConstructor.h"
#include "Procedural/Data/MeshData.h"
#include "Procedural/Data/TreeConstructionData.h"
#include "Procedural/Data/CylinderData.h"
#include "Procedural/Branch.h"
#include "MeshDataConstructor.h"
#include "BranchUtility.h"

#include "ProceduralMeshComponent.h"
#include "KismetProceduralMeshLibrary.h"


void UMeshConstructor::GenerateTreeMesh(FTreeConstructionData* TreeConstructionData) {
	FMeshData* allMeshData = TreeConstructionData->AllMeshData;
	if (TreeConstructionData->Mesh->GetNumSections() > 1) {
		TreeConstructionData->Mesh->ClearAllMeshSections();
	}
	allMeshData->Reset();

	TArray<FBranch*> allBranches = UBranchUtility::RecursiveGetAllBranchesAsArray(TreeConstructionData->RootBranch);
	UE_LOG(LogTemp, Warning, TEXT("Number of Branches: %d"), allBranches.Num());

	if (TreeConstructionData->MakeFractalMesh) {
		GenerateFractalMesh(TreeConstructionData, allBranches);
	}
	else {
		UBranchUtility::CalcTreeLikeConnectionNormals(TreeConstructionData->RootBranch);
	}

	float rootMaxRadius = TreeConstructionData->BranchRadiusZero;
	if (TreeConstructionData->DoBranchRadiusCalculations) {
		UBranchUtility::RecursiveCalculateAllBranchRadii(TreeConstructionData->RootBranch,
			TreeConstructionData->BranchRadiusZero, TreeConstructionData->BranchRadiusGrowthParameter);
		TreeConstructionData->RootBranch->StartRadius *= TreeConstructionData->TrunkRadiusMultiplier;
		rootMaxRadius = TreeConstructionData->RootBranch->StartRadius;
	}
	else {
		for (FBranch* currentBranch : allBranches) {
			currentBranch->StartRadius = rootMaxRadius;
			currentBranch->EndRadius = rootMaxRadius;
		}
	}
	
	float zRotationAngleInRad = TreeConstructionData->ZRotationAnglePerBranch * PI / 180.0f;
	UBranchUtility::CalcPerBranchDepthZRotAngle(TreeConstructionData->RootBranch, zRotationAngleInRad);

	
	
	int vertexCounter = 0;
	int meshSectionCount = 0;
	while (allBranches.Num() > 0) {
		FBranch* currentBranch = allBranches[0];
		GenerateBranchMesh(allMeshData, currentBranch, allBranches, rootMaxRadius,
			TreeConstructionData->MinNumberOfSectionsPerBranch, TreeConstructionData->MaxNumberOfSectionsPerBranch);

		if (allMeshData->Vertices.Num() > TreeConstructionData->MaxNumberOfVerticesPerMeshSection) {
			TreeConstructionData->Mesh->CreateMeshSection(meshSectionCount, allMeshData->Vertices, 
				allMeshData->Triangles, allMeshData->Normals, allMeshData->UVs, TArray<FColor>(), 
				allMeshData->Tangents, false);
			meshSectionCount++;
			vertexCounter += allMeshData->Vertices.Num();
			allMeshData->Reset();
		}
	}
	TreeConstructionData->Mesh->CreateMeshSection(meshSectionCount, allMeshData->Vertices,
		allMeshData->Triangles, allMeshData->Normals, allMeshData->UVs, TArray<FColor>(), 
		allMeshData->Tangents,
		false);
	meshSectionCount++;
	vertexCounter += allMeshData->Vertices.Num();
	allMeshData->Reset();

	UE_LOG(LogTemp, Warning, TEXT("Generated %d Vertices"), vertexCounter);
	UE_LOG(LogTemp, Warning, TEXT("Generated %d MeshSections"), TreeConstructionData->Mesh->GetNumSections());
}

void UMeshConstructor::GenerateFractalMesh(FTreeConstructionData* TreeConstructionData, const TArray<FBranch*>& AllBranches) {
	UBranchUtility::CalcConnectionNormals(TreeConstructionData->RootBranch);
	for (FBranch* currentBranch : AllBranches) {
		currentBranch->BranchDepth++;
	}
}

void UMeshConstructor::GenerateBranchMesh(FMeshData* AllMeshData, FBranch* Origin, TArray<FBranch*>& AllBranches, float RootRadius,
	int MinNumberOfSectionsPerBranch, int MaxNumberOfSectionsPerBranch) {
	TArray<FBranch*> BranchesOnSameDepth = UBranchUtility::RecursiveGetAllBranchesOnSameDepth(Origin);
	for (FBranch* currentBranch : BranchesOnSameDepth) {
		AllBranches.Remove(currentBranch);
	}
	//UE_LOG(LogTemp, Warning, TEXT("Num Of Branches as one cylinder: %d"), BranchesOnSameDepth.Num());

	FCylinderData CylinderData;

	CylinderData.RingCenters.Add(Origin->Start);
	CylinderData.ConnectionNormals.Add(Origin->StartConnectionNormal);
	CylinderData.RingRadii.Add(Origin->StartRadius);
	
	for (FBranch* currentBranch : BranchesOnSameDepth) {
		CylinderData.RingCenters.Add(currentBranch->End);
		CylinderData.RingRadii.Add(currentBranch->EndRadius);
		CylinderData.ConnectionNormals.Add(currentBranch->EndConnectionNormal);
	}

	int NumberOfSectionsUsed = UBranchUtility::CalcNumOfBranchSections(CylinderData.RingRadii[0], RootRadius, MinNumberOfSectionsPerBranch, MaxNumberOfSectionsPerBranch);

	CylinderData.NumberOfMeshSections = NumberOfSectionsUsed;
	CylinderData.LocalZRotationAngle = Origin->ZRotationAngle;

	UMeshDataConstructor::GenerateMultiLevelCylinder(*AllMeshData, CylinderData);
}

