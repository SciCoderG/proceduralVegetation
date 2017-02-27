// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "Bachelor/Procedural/Data/TreeConstructionData.h"
#include "Data/LSystemData.h"
#include "LSystemPlant.generated.h"

class UProceduralMeshComponent;
class ATurtleInterpreter;
struct FBranch;
struct FMeshData;
struct FTreeConstructionData;

DECLARE_STATS_GROUP(TEXT("LSystemPlant"), STATGROUP_LSystem, STATCAT_Advanced);

UCLASS()
class BACHELOR_API ALSystemPlant : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ALSystemPlant();

	virtual ~ALSystemPlant();

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	// Called every frame
	virtual void Tick( float DeltaSeconds ) override;

#if WITH_EDITOR  
	virtual void PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent);
#endif

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Procedural|Visual")
		UProceduralMeshComponent* Mesh;


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Procedural|LSystem")
		FLSystemData LSystemData;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Procedural|TreeConstruction")
		FTreeConstructionData TreeConstructionData;

private:

	void InitUtilityValues();

	void ConstructDataMaps();
	void ReplaceConstantIDsByValues();
	void ConstructProductionMap();

	void RemoveAnyWhiteSpaces();

	void CompleteDerivation();
	FString Derivate(FString ToDerivate);

	FString CheckProduction(FProductionData* Production, int KeyIndex, int& OutNumberOfCharsToIgnore);
	bool FillParameterValues(TMap<FString, FString> &OutParameterValues, FProductionData* Production, FString ContentBetweenBrackets);

	TMap<TCHAR, FProductionData> ProductionMap;
	
	FString CurrentDerivation;

	ATurtleInterpreter* turtleInterpreter;

	FMeshData* AllMeshData;
	FBranch* RootBranch;
};
