#pragma once

#include "UnrealMath.h"
#include "LSConstantData.generated.h"

struct FBranch;

USTRUCT()
struct BACHELOR_API FLSConstantData
{
	GENERATED_USTRUCT_BODY()
public:

	FLSConstantData() {
		
	}
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Constant")
		FString ID;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Constant")
		float ConstantValue;
	
};

