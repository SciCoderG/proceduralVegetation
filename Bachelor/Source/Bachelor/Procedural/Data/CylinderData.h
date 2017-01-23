#pragma once

#include "UnrealMath.h"
#include "CylinderData.generated.h"

USTRUCT()
struct BACHELOR_API FCylinderData
{
	GENERATED_USTRUCT_BODY()
public:

	FCylinderData() {
		
	}

	TArray<FVector> RingCenters;
	TArray<FVector> ConnectionNormals;
	TArray<float> RingRadii;

	int NumberOfMeshSections;
	float LocalZRotationAngle;
};

