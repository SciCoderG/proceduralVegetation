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
#if WITH_EDITOR  
	virtual void PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent);
#endif

	virtual float GetMaxDistanceFromCenter() PURE_VIRTUAL(AColonizationSpace::GetMaxDistanceFromCenter, return 0;);

	virtual void GenerateRandomColonizationPoints() PURE_VIRTUAL(AColonizationSpace::GenerateRandomColonizationPoints, );

	TSet<FVector>& GetColonizationPoints();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Procedural")
		int NumberOfGenerationPoints;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Procedural")
		int RandomSeed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Procedural|Debug")
		bool DrawDebugPoints;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Procedural|Debug")
		FColor DebugPointColor;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Procedural|Debug")
		float DebugPointSize;
protected:

	void DrawDebugColonizationPoints();

	TSet<FVector> ColonizationPoints;
	FRandomStream RandomStream;
};
