#pragma once

#include "UnrealMath.h"
#include "TurtleState.generated.h"

struct FBranch;

USTRUCT()
struct BACHELOR_API FTurtleState
{
	GENERATED_USTRUCT_BODY()
public:

	FTurtleState() {
		
	}

	FVector StatePosition;
	FQuat StateRotation;
	FBranch* StateBranch;
};

