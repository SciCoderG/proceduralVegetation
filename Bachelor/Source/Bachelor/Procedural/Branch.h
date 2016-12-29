#pragma once

#include "UnrealMath.h"
#include "Branch.generated.h"

USTRUCT()
struct BACHELOR_API FBranch
{
	GENERATED_USTRUCT_BODY()
public:
	FBranch() {
		ParentBranch = NULL;

		Start = FVector(0);
		End = FVector(0);

		GrowDirection = FVector(0);

		GrowCount = 0;
	}

	FVector Start;
	FVector End;

	FVector GrowDirection;
	int GrowCount;

	FBranch* ParentBranch;
	TArray<FBranch*> ChildBranches;
	
};

