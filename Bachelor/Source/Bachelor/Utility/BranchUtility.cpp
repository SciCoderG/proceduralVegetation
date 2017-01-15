// Fill out your copyright notice in the Description page of Project Settings.

#include "Bachelor.h"
#include "BranchUtility.h"
#include "Procedural/Branch.h"


TSet<FBranch*> UBranchUtility::RecursiveGetAllBranches(FBranch* Parent) {
	TSet<FBranch*> parentAndChildren;
	parentAndChildren.Add(Parent);
	for (FBranch* childBranch : Parent->ChildBranches) {
		parentAndChildren.Append(RecursiveGetAllBranches(childBranch));
	}
	return parentAndChildren;
}

TArray<FBranch*> UBranchUtility::RecursiveGetAllBranchesOnSameDepth(FBranch* Parent) {
	TArray<FBranch*> BranchesOnSameDepth;
	for (FBranch* childBranch : Parent->ChildBranches) {
		if (childBranch->BranchDepth == Parent->BranchDepth) {
			BranchesOnSameDepth.Add(childBranch);
			BranchesOnSameDepth.Append(RecursiveGetAllBranchesOnSameDepth(childBranch));
		}
	}
	return BranchesOnSameDepth;
}

TSet<FBranch*> UBranchUtility::RecursiveGetAllLeaves(FBranch* Parent) {
	TSet<FBranch*> leaves;
	for (FBranch* potentialLeaf : Parent->ChildBranches) {
		if (potentialLeaf->ChildBranches.Num() < 1) {
			leaves.Add(potentialLeaf);
		}
		else {
			leaves.Append(RecursiveGetAllLeaves(potentialLeaf));
		}
	}
	return leaves;
}

float UBranchUtility::RecursiveCalculateAllBranchRadii(FBranch* Parent, float RadiusZero, float BranchingRadiusParameter) {
	if (Parent->ChildBranches.Num() < 1) {
		Parent->StartRadius = RadiusZero;
		Parent->EndRadius = RadiusZero;
		return RadiusZero;
	}
	else {
		float maxChildBranchRadius = 0.f;
		float nthPowerOfEndRadius = 0.f;
		for (FBranch* childBranch : Parent->ChildBranches) {
			float childBranchEndRadius = RecursiveCalculateAllBranchRadii(childBranch, RadiusZero, BranchingRadiusParameter);
			if (childBranchEndRadius > maxChildBranchRadius) {
				maxChildBranchRadius = childBranchEndRadius;
			}
			nthPowerOfEndRadius += FMath::Pow(childBranchEndRadius, BranchingRadiusParameter);
		}
		float calculatedEndRadius = FMath::Pow(nthPowerOfEndRadius, 1.0f / BranchingRadiusParameter); // nth Root of x = x ^ (1 / n)

		Parent->StartRadius = calculatedEndRadius;
		Parent->EndRadius = maxChildBranchRadius;
		return calculatedEndRadius;
	}
}

void UBranchUtility::RecursiveDeleteAllBranches(FBranch* Parent) {
	for (FBranch* childBranch : Parent->ChildBranches) {
		RecursiveDeleteAllBranches(childBranch);
	}
	delete Parent;
}