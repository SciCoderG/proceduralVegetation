// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "ColonizationSpace.generated.h"

UCLASS()
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

	TSet<FVector>* GetColonizationPoints();

	float GetMaxDistanceFromCenter();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Procedural)
		class USphereComponent* ColonizationSphere;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Procedural)
		float ColonizationRadius;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Procedural)
		float NumberOfGenerationPoints;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Procedural)
		int RandomSeed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Debug)
		bool DrawDebugPoints;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Debug)
		FColor DebugPointColor;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Debug)
		float DebugPointSize;
private:

	void GenerateRandomColonizationPoints();

	void DrawDebugColonizationPoints();

	void InitValues();

	TSet<FVector> ColonizationPoints;

	FRandomStream RandomStream;
};
