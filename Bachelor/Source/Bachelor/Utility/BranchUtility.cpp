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
			break;
		}
	}
	return BranchesOnSameDepth;
}

TArray<FBranch*> UBranchUtility::RecursiveGetAllLeaves(FBranch* Parent) {
	TArray<FBranch*> leaves;
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

void UBranchUtility::CalcConnectionNormals(FBranch* Current) {
	Current->StartConnectionNormal = Current->End - Current->Start;
	Current->StartConnectionNormal = Current->StartConnectionNormal.GetUnsafeNormal();
	Current->EndConnectionNormal = Current->StartConnectionNormal;

	for (FBranch* childBranch : Current->ChildBranches) {
		CalcConnectionNormals(childBranch);
	}
}

void UBranchUtility::CalcTreeLikeConnectionNormals(FBranch* Current) {
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
		CalcTreeLikeConnectionNormals(childBranch);
	}
}

int UBranchUtility::CalcNumOfBranchSections(float CurrentRadius, float MaxRadius, int MinNumberOfSectionsPerBranch, int MaxNumberOfSectionsPerBranch) {
	if (MaxNumberOfSectionsPerBranch < 2) {
		UE_LOG(LogTemp, Warning, TEXT("UBranchUtility::CalcNumOfBranchSections: Max Number of Sections Per Branch: %d - Should be equal to or greater than 2."), MaxNumberOfSectionsPerBranch);
		MaxNumberOfSectionsPerBranch = 2;
	}
	if (MaxNumberOfSectionsPerBranch < MinNumberOfSectionsPerBranch) {
		UE_LOG(LogTemp, Warning, TEXT("UBranchUtility::CalcNumOfBranchSections: Max Number of Sections greater than Min Number - switching values"));
		int temp = MaxNumberOfSectionsPerBranch;
		MaxNumberOfSectionsPerBranch = MinNumberOfSectionsPerBranch;
		MinNumberOfSectionsPerBranch = temp;
	}

	int numberOfSectionsUsed = MaxNumberOfSectionsPerBranch;

	int diffMinMaxSections = MaxNumberOfSectionsPerBranch - MinNumberOfSectionsPerBranch ;
	int calculatedNumberOfSections = (CurrentRadius / MaxRadius) * (diffMinMaxSections + 0.5f) + MinNumberOfSectionsPerBranch;
	/*
	int CalculatedNumberOfSections = ((MaxRadius / 2.0f) - 1) + MinNumberOfSectionsPerBranch;
	*/
	if (calculatedNumberOfSections < MaxNumberOfSectionsPerBranch) {
		numberOfSectionsUsed = calculatedNumberOfSections;
	}
	
	return numberOfSectionsUsed;
}

void UBranchUtility::CalcPerBranchDepthZRotAngle(FBranch* Current, float RotationAngleIncrement){
	int numChildBranches = Current->ChildBranches.Num();
	for (int i = 0; i < numChildBranches; ++i) {
		FBranch* childBranch = Current->ChildBranches[i];
		childBranch->ZRotationAngle = Current->ZRotationAngle + RotationAngleIncrement * i;
		
		CalcPerBranchDepthZRotAngle(childBranch, RotationAngleIncrement);
	}
}

void UBranchUtility::ReduceGrownBranchesByMaxDotProduct(FBranch* Parent, float MaxDotProduct) {
	int numChildBranches = Parent->ChildBranches.Num();
	if (numChildBranches < 1) {
		return;
	}
	while (numChildBranches == 1) {
		FBranch* singleChild = Parent->ChildBranches[0];

		FVector parentDirection = Parent->End - Parent->Start;
		parentDirection = parentDirection.GetSafeNormal();

		FVector childDirection = singleChild->End - singleChild->Start;
		childDirection = childDirection.GetSafeNormal();

		if (FVector::DotProduct(parentDirection, childDirection) > MaxDotProduct) {
			MergeParentAndChild(Parent, singleChild);

			numChildBranches = singleChild->ChildBranches.Num();

			delete singleChild;
		}
		else {
			ReduceGrownBranchesByMaxDotProduct(singleChild, MaxDotProduct);
			return;
		}
	}

	for (FBranch* childBranch : Parent->ChildBranches) {
		ReduceGrownBranchesByMaxDotProduct(childBranch, MaxDotProduct);
	}
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
		
		MergeParentAndChild(Parent, singleChild);

		numChildBranches = singleChild->ChildBranches.Num();

		delete singleChild;
	}

	for (FBranch* childBranch : Parent->ChildBranches) {
		RecursiveReduceGrownBranches(childBranch);
	}
}

void UBranchUtility::SmoothOutBranchingAngles(FBranch* Current) {
	if (Current->ChildBranches.Num() > 1) {
		FVector childBranchCenter = FVector(0);
		for (FBranch* childBranch : Current->ChildBranches) {
			childBranch->Start = Current->End;
			childBranchCenter += childBranch->End - childBranch->Start;
		}
		childBranchCenter /= Current->ChildBranches.Num();
		childBranchCenter += Current->End;
		for (FBranch* childBranch : Current->ChildBranches) {
			FVector branchEndToCenter = childBranchCenter - childBranch->End;
			childBranch->End += branchEndToCenter / 2.0f;
		}
	}
	for (FBranch* childBranch : Current->ChildBranches) {
		childBranch->Start = Current->End;
		SmoothOutBranchingAngles(childBranch);
	}
}

void UBranchUtility::MergeParentAndChild(FBranch* Parent, FBranch* Child) {
	Parent->ChildBranches.Reset();
	for (FBranch* newChildBranch : Child->ChildBranches) {
		Parent->ChildBranches.Add(newChildBranch);
		newChildBranch->ParentBranch = Parent;
	}
	Parent->End = Child->End;
}
