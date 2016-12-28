// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "SpaceColonizationPlant.generated.h"

class UProceduralMeshComponent;
class AColonizationSpace;
struct FBranch;

UCLASS()
class BACHELOR_API ASpaceColonizationPlant : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASpaceColonizationPlant();

	virtual ~ASpaceColonizationPlant();

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	// Called every frame
	virtual void Tick( float DeltaSeconds ) override;

	virtual void PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Material)
		UMaterialInterface* GeneratedTreeMaterial;

	UPROPERTY(EditAnywhere, BluePrintReadOnly, Category = Procedural)
		UProceduralMeshComponent* Mesh;

	UPROPERTY(EditAnywhere, BluePrintReadOnly, Category = Procedural)
		TArray<AColonizationSpace* > GrowthSpaces;

	UPROPERTY(EditAnywhere, BluePrintReadOnly, Category = Procedural)
		float KillDistance;

	UPROPERTY(EditAnywhere, BluePrintReadOnly, Category = Procedural)
		float RadiusOfInfluence;

	UPROPERTY(EditAnywhere, BluePrintReadOnly, Category = Procedural)
		float GrowthPerIteration;

	UPROPERTY(EditAnywhere, BluePrintReadOnly, Category = Procedural)
		float MaxNumGrowthIterations;

private:
	void ColonizeGivenSpaces();

	void InitialRootGrowth();

	AColonizationSpace* GetNearestColonizationSpace();

	void GrowthIteration();

	void GenerateTreeMesh();

	FBranch* RootBranch;

	TSet<FBranch*> GrowingBranches;
	
};
