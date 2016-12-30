// Fill out your copyright notice in the Description page of Project Settings.

#include "Bachelor.h"
#include "SpaceColonizationPlant.h"

#include "ProceduralMeshComponent.h"
#include "KismetProceduralMeshLibrary.h"

#include "ColonizationSpace.h"
#include "Branch.h"
#include "Utility/MeshConstructor.h"


// Sets default values
ASpaceColonizationPlant::ASpaceColonizationPlant()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	USphereComponent* Root = CreateDefaultSubobject<USphereComponent>(TEXT("RootComponent"));
	Root->InitSphereRadius(1.0f);
	RootComponent = Root;

	Mesh = CreateDefaultSubobject<UProceduralMeshComponent>(TEXT("GeneratedMesh"));
	Mesh->SetupAttachment(RootComponent);

	RootBranch = new FBranch();
	
	KillDistance = 10.0f;

	RadiusOfInfluence = 100.f;

	GrowthPerIteration = 10.f;

	MaxNumGrowthIterations = 200.f;

	InitUtilityValues();
}

ASpaceColonizationPlant::~ASpaceColonizationPlant() {

}

// Called when the game starts or when spawned
void ASpaceColonizationPlant::BeginPlay()
{
	Super::BeginPlay();

	ColonizeGivenSpaces();
	GenerateTreeMesh();
}

// Called every frame
void ASpaceColonizationPlant::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );
}

void ASpaceColonizationPlant::PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent) {
	if (GeneratedTreeMaterial) {
		for (int i = 0; i < Mesh->GetNumSections(); ++i) {
			Mesh->SetMaterial(i, GeneratedTreeMaterial);
		}
	}

	InitUtilityValues();

	Super::PostEditChangeProperty(PropertyChangedEvent);
}

void ASpaceColonizationPlant::InitUtilityValues() {
	KillDistanceSquared = KillDistance * KillDistance;
	RadiusOfInfluenceSquared = RadiusOfInfluence * RadiusOfInfluence;
}

void ASpaceColonizationPlant::ColonizeGivenSpaces() {

	InitialRootGrowth();
	GrowingBranches.Add(RootBranch);
	for (int i = 0; i < MaxNumGrowthIterations; ++i) {
		GrowthIteration();
		if (GrowingBranches.Num() < 1) {
			UE_LOG(LogTemp, Warning, TEXT("No Growing Branches left, interrupting Colonization"));
			break;
		}
	}
}

void ASpaceColonizationPlant::InitialRootGrowth() {
	AColonizationSpace* nearestCSpace = GetNearestColonizationSpace();

	RootBranch->Start = FVector(0.f);

	if (nearestCSpace) {
		FVector plantToNearestCSpace = nearestCSpace->GetActorLocation() - this->GetActorLocation();
		float distPlantToNearestCSpace = plantToNearestCSpace.Size();

		FVector normalPlantToNearestCSpace = plantToNearestCSpace.GetSafeNormal();
		float maxDistanceToCSpaceCenter = nearestCSpace->GetMaxDistanceFromCenter();
		RootBranch->End = (distPlantToNearestCSpace - maxDistanceToCSpaceCenter) * normalPlantToNearestCSpace;
	}
}

AColonizationSpace* ASpaceColonizationPlant::GetNearestColonizationSpace() {
	AColonizationSpace* nearestColonizationSpace = NULL;
	if (GrowthSpaces.Num() < 1) {
		UE_LOG(LogTemp, Warning, TEXT("No GrowthSpaces specified in ASpaceColonizationPlant::GetNearestColonizationSpace()."));
		return nearestColonizationSpace;
	}

	FVector plantPosition = this->GetActorLocation();
	nearestColonizationSpace = GrowthSpaces[0];

	if ( NULL == nearestColonizationSpace) {
		UE_LOG(LogTemp, Warning, TEXT("First GrowthSpace is NULL"));
		return nearestColonizationSpace;
	}

	float minSquaredDistance = FVector::DistSquared(nearestColonizationSpace->GetActorLocation(), plantPosition);
	for (AColonizationSpace* currentSpace : GrowthSpaces) {
		float sqrDistancePlantToSpace = FVector::DistSquared(currentSpace->GetActorLocation(), plantPosition);

		if (sqrDistancePlantToSpace < minSquaredDistance) {
			minSquaredDistance = sqrDistancePlantToSpace;
			nearestColonizationSpace = currentSpace;
		}
	}
	return nearestColonizationSpace;
}

void ASpaceColonizationPlant::GrowthIteration() {
	TSet<FVector> allColonizationPoints;

	for (AColonizationSpace* currentSpace : GrowthSpaces) {
		if (NULL != currentSpace) {
			allColonizationPoints.Append(*(currentSpace->GetColonizationPoints()));
		}
	}

	for (FVector currentPoint : allColonizationPoints) {
		CheckColonizationPoint(currentPoint);
	}
	
	for (FBranch* currentBranch : GrowingBranches) {
		GrowBranch(currentBranch);
	}
}

void ASpaceColonizationPlant::CheckColonizationPoint(FVector ColonizationPoint) {
	// i don't want to copy this array around, so this function will be kinda big
	TArray<FBranch*> branchesInInfluenceRadius;
	for (FBranch* currentBranch : GrowingBranches) {
		FVector branchEndToPoint = ColonizationPoint - (currentBranch->End + this->GetActorLocation());
		float distancePointToBranchSquared = branchEndToPoint.SizeSquared();

		if (distancePointToBranchSquared > RadiusOfInfluenceSquared) {
			break;
		}
		else if (distancePointToBranchSquared < KillDistanceSquared) {
			RemoveFromGrowthSpaces(ColonizationPoint);
			break;
		}
		else {
			branchesInInfluenceRadius.Add(currentBranch);
		}
	}

	if (branchesInInfluenceRadius.Num() < 1) {
		return;
	}

	FBranch* nearestBranch = branchesInInfluenceRadius[0];
	FVector minDistanceVector = ColonizationPoint - (nearestBranch->End + this->GetActorLocation());
	float minDistanceSquared = minDistanceVector.SizeSquared();

	for (FBranch* currentBranch : branchesInInfluenceRadius) {
		FVector currentDistanceVector = ColonizationPoint - (currentBranch->End + this->GetActorLocation());
		float currentDistanceSquared = currentDistanceVector.SizeSquared();
		if (currentDistanceSquared < minDistanceSquared) {
			nearestBranch = currentBranch;
			minDistanceSquared = currentDistanceSquared;
			minDistanceVector = currentDistanceVector;
		}
	}

	nearestBranch->GrowCount++;
	nearestBranch->GrowDirection += minDistanceVector.GetSafeNormal(); // TODO check if this is ok, or not!!! ( dividing by minDistanceSquared)
}

void ASpaceColonizationPlant::RemoveFromGrowthSpaces(FVector ToRemove) {
	for (AColonizationSpace* currentSpace : GrowthSpaces) {
		currentSpace->GetColonizationPoints()->Remove(ToRemove);
	}
}

void ASpaceColonizationPlant::GrowBranch(FBranch* ToGrow) {
	if (ToGrow->GrowCount == 0) {
		GrowingBranches.Remove(ToGrow);
	}
	else if (ToGrow->GrowCount > 0) {
		FBranch* newBranch = new FBranch();

		ToGrow->ChildBranches.Add(newBranch);
		newBranch->ParentBranch = ToGrow;
		if (ToGrow->ChildBranches.Num() > 1) {
			for (FBranch* childBranch : ToGrow->ChildBranches) {
				childBranch->BranchDepth = ToGrow->BranchDepth + 1;
			}
		}

		newBranch->Start = ToGrow->End;

		FVector normalizedGrowthDirection = ToGrow->GrowDirection / ToGrow->GrowCount;
		normalizedGrowthDirection = normalizedGrowthDirection.GetSafeNormal();
		newBranch->End = newBranch->Start + normalizedGrowthDirection * GrowthPerIteration;

		GrowingBranches.Add(newBranch);
	}

	ToGrow->ResetForNextGrowthIteration();
}

void ASpaceColonizationPlant::GenerateTreeMesh() {


	if (Mesh->GetNumSections() > 1) {
		Mesh->ClearAllMeshSections();
	}

	TSet<FBranch*> allBranches = RecursiveGetAllBranches(RootBranch);

	
	int i = 0;
	for (FBranch* currentBranch : allBranches) {
		FMeshData& meshData = currentBranch->MeshData;

		UMeshConstructor::GenerateCylinder(meshData, currentBranch->Start, 1.0f, currentBranch->End, 1.0f, 16);
		Mesh->CreateMeshSection(i, meshData.Vertices, meshData.Triangles, meshData.Normals, meshData.UVs, TArray<FColor>(), meshData.Tangents, false);
		Mesh->SetMaterial(i, GeneratedTreeMaterial);
		++i;
	}
	UE_LOG(LogTemp, Warning, TEXT("Generated %d MeshSections"), i);
	
	/*FMeshData meshData;
	UMeshConstructor::GenerateCylinder(meshData, FVector(0,0,0), 5.0f, FVector(0,0,20), 5.0f, 16);
	Mesh->CreateMeshSection(0, meshData.Vertices, meshData.Triangles, meshData.Normals, meshData.UVs, TArray<FColor>(), meshData.Tangents, false);
	Mesh->SetMaterial(0, GeneratedTreeMaterial);*/
	
}


TSet<FBranch*> ASpaceColonizationPlant::RecursiveGetAllBranches(FBranch* Parent) {
	TSet<FBranch*> parentAndChildren;
	parentAndChildren.Add(Parent);
	for (FBranch* childBranch : Parent->ChildBranches) {
		parentAndChildren.Append(RecursiveGetAllBranches(childBranch));
	}
	return parentAndChildren;
}
