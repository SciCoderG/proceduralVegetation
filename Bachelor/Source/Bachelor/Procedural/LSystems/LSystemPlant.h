// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "Bachelor/Procedural/Data/TreeConstructionData.h"
#include "Data/LSystemData.h"
#include "LSystemPlant.generated.h"

class UProceduralMeshComponent;
struct FBranch;
struct FMeshData;
struct FTreeConstructionData;

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

	virtual void PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent);


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Procedural|Visual")
		UMaterialInterface* GeneratedTreeMaterial;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Procedural|Visual")
		UProceduralMeshComponent* Mesh;


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Procedural|LSystem")
		FLSystemData LSystemData;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Procedural|TreeConstruction")
		FTreeConstructionData TreeConstructionData;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Procedural|TreeConstruction")
		bool SmoothOutBranchingAngles;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Procedural|TreeConstruction")
		bool PolyReductionByCurveReduction;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Procedural|TreeConstruction")
		FVector Tropism;


private:

	void InitUtilityValues();

	void ConstructProductionMap();
	void ConstructFunctionMap();

	void CompleteDerivation();

	void Derivate();

	// https://wiki.unrealengine.com/Function_Pointers
	typedef void (ALSystemPlant::*FunctionPtrType)(void);
	TMap<FString, FunctionPtrType> FunctionMap;

	TMap<TCHAR, FProductionData> ProductionMap;
	
	FString CurrentDerivation;

	FMeshData* AllMeshData;
	
	FBranch* RootBranch;
};
