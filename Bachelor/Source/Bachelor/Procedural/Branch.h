#pragma once

#include "UnrealMath.h"
#include "Branch.generated.h"

USTRUCT()
struct FBranch
{
	GENERATED_USTRUCT_BODY()
public:
	FBranch() {
		ParentBranch = NULL;

		GrowCount = 0;
	}

	FVector Start;
	FVector End;

	FVector GrowDirection;
	int GrowCount;

	FBranch* ParentBranch;
	TArray<FBranch*> ChildBranches;
	
};

