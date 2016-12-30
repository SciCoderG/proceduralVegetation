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
	
	KillDistance = 100.0f;

	RadiusOfInfluence = 500.0f;

	GrowthPerIteration = 10.0f;

	MaxNumGrowthIterations = 20.0f;

	InitUtilityValues();

	MeshSectionToChange = 0;
}

ASpaceColonizationPlant::~ASpaceColonizationPlant() {

}

// Called when the game starts or when spawned
void ASpaceColonizationPlant::BeginPlay()
{
	Super::BeginPlay();

	UE_LOG(LogTemp, Warning, TEXT("Name: %s"), *this->GetName());
	ColonizeGivenSpaces();
	GenerateTreeMesh();

	
}

// Called every frame
void ASpaceColonizationPlant::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );
}

void ASpaceColonizationPlant::PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent) {

	InitUtilityValues();
	FName PropertyName = (PropertyChangedEvent.Property != NULL) ? PropertyChangedEvent.Property->GetFName() : NAME_None;

	if ((PropertyName == GET_MEMBER_NAME_CHECKED(ASpaceColonizationPlant, MaxNumGrowthIterations))) {
		ColonizeGivenSpaces();
		GenerateTreeMesh();
	}
	
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
	UE_LOG(LogTemp, Warning, TEXT("Finished max number of growth iterations with %d growing Branches left"), GrowingBranches.Num());
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
	else {
		FVector normalizedGrowthDirection = ToGrow->GrowDirection / ToGrow->GrowCount;
		normalizedGrowthDirection = normalizedGrowthDirection.GetSafeNormal();

		if (ToGrow->GrowCount == 1 && ToGrow->ChildBranches.Num() < 1) {
			ToGrow->End = ToGrow->End + normalizedGrowthDirection * GrowthPerIteration;
		}
		else if (ToGrow->GrowCount > 0) {
			FBranch* newBranch = new FBranch();
			ToGrow->ChildBranches.Add(newBranch);
			newBranch->ParentBranch = ToGrow;
			if (ToGrow->ChildBranches.Num() > 1) {
				newBranch->BranchDepth = ToGrow->BranchDepth + 1;
			}
			else {
				newBranch->BranchDepth = ToGrow->BranchDepth;
			}
			newBranch->Start = ToGrow->End;
			newBranch->End = newBranch->Start + normalizedGrowthDirection * GrowthPerIteration;
			GrowingBranches.Add(newBranch);
		}
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
		GenerateBranchMesh(currentBranch, allBranches, i);
		++i;
	}

	UE_LOG(LogTemp, Warning, TEXT("Generated %d MeshSections"), Mesh->GetNumSections());
}

TArray<FBranch*> RecursiveGetAllBranchesOnSameDepth(FBranch* Parent) {
	TArray<FBranch*> BranchesOnSameDepth;
	for (FBranch* childBranch : Parent->ChildBranches) {
		if (childBranch->BranchDepth == Parent->BranchDepth) {
			BranchesOnSameDepth.Add(childBranch);
			BranchesOnSameDepth.Append(RecursiveGetAllBranchesOnSameDepth(childBranch));
			break;
		}
	}
	return BranchesOnSameDepth;
}

void ASpaceColonizationPlant::GenerateBranchMesh(FBranch* Origin, TSet<FBranch*>& AllBranches, int MeshSection) {
	TArray<FBranch*> BranchesOnSameDepth = RecursiveGetAllBranchesOnSameDepth(Origin);
	for (FBranch* currentBranch : BranchesOnSameDepth) {
		AllBranches.Remove(currentBranch);
	}
	
	float MaxRingRadius = 5.0f;
	MaxRingRadius /= (Origin->BranchDepth + 1);

	TArray<FVector> RingCenters;
	RingCenters.Add(Origin->Start);
	RingCenters.Add(Origin->End);

	TArray<float> RingRadii;
	RingRadii.Add(MaxRingRadius );
	RingRadii.Add(MaxRingRadius );
	for (FBranch* currentBranch : BranchesOnSameDepth) {
		RingCenters.Add(currentBranch->End);
		RingRadii.Add(MaxRingRadius);
	}
	
	FMeshData branchData;
	UMeshConstructor::GenerateMultiLevelCylinder(branchData, RingCenters, RingRadii, 8);

	Mesh->CreateMeshSection(MeshSection, branchData.Vertices, branchData.Triangles, branchData.Normals, 
		branchData.UVs, TArray<FColor>(), branchData.Tangents, false);
	Mesh->SetMaterial(MeshSection, GeneratedTreeMaterial);
}

TSet<FBranch*> ASpaceColonizationPlant::RecursiveGetAllBranches(FBranch* Parent) {
	TSet<FBranch*> parentAndChildren;
	parentAndChildren.Add(Parent);
	for (FBranch* childBranch : Parent->ChildBranches) {
		parentAndChildren.Append(RecursiveGetAllBranches(childBranch));
	}
	return parentAndChildren;
}
