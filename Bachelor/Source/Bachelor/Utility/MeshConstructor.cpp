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



DECLARE_CYCLE_STAT(TEXT("UMeshConstructor ~ GenerateTreeMesh"), STAT_GenerateTreeMesh, STATGROUP_UMeshConstructor);
void UMeshConstructor::GenerateTreeMesh(FTreeConstructionData* TreeConstructionData) {
	SCOPE_CYCLE_COUNTER(STAT_GenerateTreeMesh);
	FMeshData* allMeshData = TreeConstructionData->AllMeshData;
	if (TreeConstructionData->Mesh->GetNumSections() > 1) {
		TreeConstructionData->Mesh->ClearAllMeshSections();
	}
	allMeshData->Reset();

	if (TreeConstructionData->MaxDotProductBetweenParentAndChild > 0.f && TreeConstructionData->MaxDotProductBetweenParentAndChild < 1.0f) {
		UBranchUtility::ReduceGrownBranchesByMaxDotProduct(TreeConstructionData->RootBranch, TreeConstructionData->MaxDotProductBetweenParentAndChild);
	}

	TArray<FBranch*> allBranches = UBranchUtility::RecursiveGetAllBranchesAsArray(TreeConstructionData->RootBranch);

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

	if (TreeConstructionData->MakeFractalMesh) {
		GenerateFractalMesh(TreeConstructionData, allBranches);
		if (allBranches.Num() > 1) {
			allBranches.RemoveAt(0);
		}
	}
	else {
		UBranchUtility::CalcTreeLikeConnectionNormals(TreeConstructionData->RootBranch);
	}
	
	UE_LOG(LogTemp, Warning, TEXT("Final Number of Branches: %d"), allBranches.Num());

	int vertexCounter = 0;
	int meshSectionCount = 0;
	for (FBranch* currentBranch : allBranches) {
		if (!currentBranch->IsConstructed) {
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
	}
	TreeConstructionData->Mesh->CreateMeshSection(meshSectionCount, allMeshData->Vertices,
		allMeshData->Triangles, allMeshData->Normals, allMeshData->UVs, TArray<FColor>(), 
		allMeshData->Tangents,
		false);
	meshSectionCount++;
	vertexCounter += allMeshData->Vertices.Num();
	allMeshData->Reset();

	if (NULL != TreeConstructionData->TreeMaterial) {
		for (int i = 0; i < TreeConstructionData->Mesh->GetNumSections(); ++i) {
			TreeConstructionData->Mesh->SetMaterial(i, TreeConstructionData->TreeMaterial);
		}
	}

	UE_LOG(LogTemp, Warning, TEXT("Generated %d Vertices"), vertexCounter);
	UE_LOG(LogTemp, Warning, TEXT("Generated %d MeshSections"), TreeConstructionData->Mesh->GetNumSections());
}

void UMeshConstructor::GenerateFractalMesh(FTreeConstructionData* TreeConstructionData, const TArray<FBranch*>& AllBranches) {
	UBranchUtility::CalcConnectionNormals(TreeConstructionData->RootBranch);
	int i = 0;
	for (FBranch* currentBranch : AllBranches) {
		currentBranch->BranchDepth = i++;
		currentBranch->Start -= currentBranch->StartConnectionNormal * currentBranch->StartRadius;
		currentBranch->End += currentBranch->EndConnectionNormal * currentBranch->EndRadius;
		currentBranch->ZRotationAngle = TreeConstructionData->ZRotationAnglePerBranch;
	}

}

DECLARE_CYCLE_STAT(TEXT("UMeshConstructor ~ GenerateBranchMesh"), STAT_GenerateBranchMesh, STATGROUP_UMeshConstructor);
DECLARE_CYCLE_STAT(TEXT("UMeshConstructor ~ GenerateBranchMesh_FindAndRemoveBranches"), GenerateBranchMesh_FindAndRemoveBranches, STATGROUP_UMeshConstructor);
DECLARE_CYCLE_STAT(TEXT("UMeshConstructor ~ GenerateBranchMesh_ConstructCylinderData"), GenerateBranchMesh_ConstructCylinderData, STATGROUP_UMeshConstructor);
void UMeshConstructor::GenerateBranchMesh(FMeshData* AllMeshData, FBranch* Origin, TArray<FBranch*>& AllBranches, float RootRadius,
	int MinNumberOfSectionsPerBranch, int MaxNumberOfSectionsPerBranch) {
	SCOPE_CYCLE_COUNTER(STAT_GenerateBranchMesh);
	TArray<FBranch*> BranchesOnSameDepth;
	{
		SCOPE_CYCLE_COUNTER(GenerateBranchMesh_FindAndRemoveBranches);
		BranchesOnSameDepth = UBranchUtility::RecursiveGetAllBranchesOnSameDepth(Origin);
		for (FBranch* currentBranch : BranchesOnSameDepth) {
			currentBranch->IsConstructed = true;
		}
		//UE_LOG(LogTemp, Warning, TEXT("Num Of Branches as one cylinder: %d"), BranchesOnSameDepth.Num());
	}
	
	FCylinderData CylinderData;
	{
		SCOPE_CYCLE_COUNTER(GenerateBranchMesh_ConstructCylinderData);

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
	}
	
	UMeshDataConstructor::GenerateMultiLevelCylinder(*AllMeshData, CylinderData);
}

