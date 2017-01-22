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

TArray<FBranch*> UBranchUtility::RecursiveGetAllBranchesAsArray(FBranch* Parent) {
	TArray<FBranch*> parentAndChildren;
	parentAndChildren.Add(Parent);
	for (FBranch* childBranch : Parent->ChildBranches) {
		parentAndChildren.Append(RecursiveGetAllBranchesAsArray(childBranch));
	}
	return parentAndChildren;
}

TArray<FBranch*> UBranchUtility::RecursiveGetAllBranchesOnSameDepth(FBranch* Parent) {
	TArray<FBranch*> BranchesOnSameDepth;
	BranchesOnSameDepth.Add(Parent);
	for (FBranch* childBranch : Parent->ChildBranches) {
		if (childBranch->BranchDepth == Parent->BranchDepth) {
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

void UBranchUtility::CalcAllBranchConnectionNormals(FBranch* Current) {
	FBranch* Parent = Current->ParentBranch;
	if (NULL != Parent) {
		Current->StartConnectionNormal = Parent->EndConnectionNormal;
	}
	else {
		Current->StartConnectionNormal = FVector(0.f, 0.f, 1.0f);
	}

	Current->EndConnectionNormal = Current->StartConnectionNormal;
	Current->EndConnectionNormal += Current->End - Current->Start;
	Current->EndConnectionNormal = Current->EndConnectionNormal.GetSafeNormal();

	for (FBranch* childBranch : Current->ChildBranches) {
		CalcAllBranchConnectionNormals(childBranch);
	}
}

int UBranchUtility::CalcNumOfBranchSections(float MaxRadius, int MinNumberOfSectionsPerBranch, int MaxNumberOfSectionsPerBranch) {
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

void UBranchUtility::RecursiveReduceGrownBranches(FBranch* Parent) {
	ElongateGrownBranches(Parent);

}

void UBranchUtility::RecursiveDeleteAllBranches(FBranch* Parent) {
	for (FBranch* childBranch : Parent->ChildBranches) {
		RecursiveDeleteAllBranches(childBranch);
	}
	delete Parent;
}


void UBranchUtility::ElongateGrownBranches(FBranch* Parent) {
	if (Parent->ChildBranches.Num() < 1) {
		return;
	}

	int numChildBranches = Parent->ChildBranches.Num();
	while (numChildBranches == 1) {
		FBranch* singleChild = Parent->ChildBranches[0];

		Parent->ChildBranches.Reset();
		for (FBranch* newChildBranch : singleChild->ChildBranches) {
			Parent->ChildBranches.Add(newChildBranch);
			newChildBranch->ParentBranch = Parent;
		}
		Parent->End = singleChild->End;
		numChildBranches = singleChild->ChildBranches.Num();

		delete singleChild;
	}

	for (FBranch* childBranch : Parent->ChildBranches) {
		RecursiveReduceGrownBranches(childBranch);
	}
}


void UBranchUtility::ReduceInnerLeafBranches(FBranch* Parent) {
	if (Parent->ChildBranches.Num() > 1) {

	}
}