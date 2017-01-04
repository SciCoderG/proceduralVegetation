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

void UBranchUtility::RecursiveDeleteAllBranches(FBranch* Parent) {
	for (FBranch* childBranch : Parent->ChildBranches) {
		RecursiveDeleteAllBranches(childBranch);
	}
	delete Parent;
}