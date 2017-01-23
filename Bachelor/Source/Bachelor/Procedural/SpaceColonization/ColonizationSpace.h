// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "ColonizationSpace.generated.h"

UCLASS(abstract)
class BACHELOR_API AColonizationSpace : public AActor
{
	GENERATED_BODY()
public:	
	// Sets default values for this actor's properties
	AColonizationSpace();

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	// Called every frame
	virtual void Tick( float DeltaSeconds ) override;
	
	// Called on property changes
	virtual void PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent);

	virtual float GetMaxDistanceFromCenter() PURE_VIRTUAL(AColonizationSpace::GetMaxDistanceFromCenter, return 0;);

	TSet<FVector>* GetColonizationPoints();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Procedural")
		float NumberOfGenerationPoints;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Procedural")
		int RandomSeed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Procedural|Debug")
		bool DrawDebugPoints;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Procedural|Debug")
		FColor DebugPointColor;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Procedural|Debug")
		float DebugPointSize;
protected:
	virtual void GenerateRandomColonizationPoints() PURE_VIRTUAL(AColonizationSpace::GenerateRandomColonizationPoints, );
	virtual void InitValues() PURE_VIRTUAL(AColonizationSpace::InitValues, );

	void DrawDebugColonizationPoints();

	TSet<FVector> ColonizationPoints;
	FRandomStream RandomStream;
};
