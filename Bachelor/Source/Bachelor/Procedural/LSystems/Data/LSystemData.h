#pragma once

#include "UnrealMath.h"
#include "ProductionData.h"
#include "LSystemData.generated.h"

USTRUCT()
struct BACHELOR_API FLSystemData
{
	GENERATED_USTRUCT_BODY()
public:

	FLSystemData() {
		NumberOfDerivations = 5;
	}

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Parameters")
		int NumberOfDerivations;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Parameters")
		FString Axiom;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Parameters")
		TArray<FProductionData> Productions;

};

