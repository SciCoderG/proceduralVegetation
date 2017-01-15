// Fill out your copyright notice in the Description page of Project Settings.

#include "Bachelor.h"
#include "SpaceColonizationPlant.h"

#include "ProceduralMeshComponent.h"
#include "KismetProceduralMeshLibrary.h"

#include "ColonizationSpace.h"
#include "Branch.h"
#include "Utility/MeshDataConstructor.h"
#include "Utility/MeshConstructor.h"
#include "Utility/BranchUtility.h"


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

	MaxNumberOfSectionsPerBranch = 12;
	KillDistance = 100.0f;
	RadiusOfInfluence = 500.0f;
	GrowthPerIteration = 10.0f;
	MaxNumGrowthIterations = 20;
	MaxNumberOfBranchingTwigs = 4;
	MaxGrowthDepth = 6;
	RootBranchRadius = 5.0f;
	MaxNumberOfVerticesPerMeshSection = 400;

	BranchRadiusZero = 1.0f;
	BranchRadiusGrowthParameter = 2.0f;
	RootBranch = new FBranch();

	InitUtilityValues();
}

ASpaceColonizationPlant::~ASpaceColonizationPlant() {

}

// Called when the game starts or when spawned
void ASpaceColonizationPlant::BeginPlay()
{
	Super::BeginPlay();

	UE_LOG(LogTemp, Warning, TEXT("Name: %s"), *this->GetName());
	ColonizeGivenSpaces();
	UMeshConstructor::GenerateTreeMesh(Mesh, AllMeshData, RootBranch, MaxNumberOfSectionsPerBranch, MaxNumberOfVerticesPerMeshSection, BranchRadiusZero, BranchRadiusGrowthParameter);
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
		// Potential place to regenerate tree
	}
	
	Super::PostEditChangeProperty(PropertyChangedEvent);
}

void ASpaceColonizationPlant::InitUtilityValues() {
	KillDistanceSquared = KillDistance * KillDistance;
	RadiusOfInfluenceSquared = RadiusOfInfluence * RadiusOfInfluence;
}

void ASpaceColonizationPlant::ColonizeGivenSpaces() {
	if (RootBranch != NULL) {
		UBranchUtility::RecursiveDeleteAllBranches(RootBranch);
	}
	RootBranch = new FBranch();
	InitialRootGrowth();
	GrowingBranches.Add(RootBranch);
	for (int i = 0; i < MaxNumGrowthIterations; ++i) {
		GrowthIteration();
		if (GrowingBranches.Num() < 1) {
			UE_LOG(LogTemp, Warning, TEXT("No Growing Branches left, interrupting Colonization"));
			break;
		}
		if (AllColonizationPoints.Num() < 1) {
			UE_LOG(LogTemp, Warning, TEXT("No Colonization Points left, interrupting Colonization"));
			break;
		}
	}
	UE_LOG(LogTemp, Warning, TEXT("Finished Space Colonization with %d growing Branches left"), GrowingBranches.Num());
}

void ASpaceColonizationPlant::InitialRootGrowth() {
	AColonizationSpace* nearestCSpace = GetNearestColonizationSpace();

	RootBranch->Start = FVector(0.f);

	if (nearestCSpace) {
		FVector plantToNearestCSpace = nearestCSpace->GetActorLocation() - this->GetActorLocation();
		float distPlantToNearestCSpace = plantToNearestCSpace.Size();

		float maxDistanceToCSpaceCenter = nearestCSpace->GetMaxDistanceFromCenter();

		if (distPlantToNearestCSpace > maxDistanceToCSpaceCenter) {
			// plant is outside of CSpace Center
			FVector normalPlantToNearestCSpace = plantToNearestCSpace.GetSafeNormal();
			RootBranch->End = (distPlantToNearestCSpace - maxDistanceToCSpaceCenter) * normalPlantToNearestCSpace;
		}
		else {
			RootBranch->End = FVector(0.f);
			RootBranch->GrowCount = 1;
			RootBranch->ChildBranches.Empty();
		}
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
	CheckAllColonizationPoints();
	GrowAllBranches();
}

TSet<FVector>& ASpaceColonizationPlant::GetAllColonizationPoints() {
	AllColonizationPoints.Empty();
	for (AColonizationSpace* currentSpace : GrowthSpaces) {
		if (NULL != currentSpace) {
			AllColonizationPoints.Append(*(currentSpace->GetColonizationPoints()));
		}
	}
	return AllColonizationPoints;
}

void ASpaceColonizationPlant::CheckAllColonizationPoints() {
	TSet<FVector> allColonizationPoints = GetAllColonizationPoints();
	for (FVector currentPoint : allColonizationPoints) {
		CheckColonizationPoint(currentPoint);
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
	nearestBranch->GrowDirection += minDistanceVector.GetSafeNormal();
}

void ASpaceColonizationPlant::RemoveFromGrowthSpaces(FVector ToRemove) {
	for (AColonizationSpace* currentSpace : GrowthSpaces) {
		currentSpace->GetColonizationPoints()->Remove(ToRemove);
	}
}

void ASpaceColonizationPlant::GrowAllBranches() {
	for (FBranch* currentBranch : GrowingBranches) {
		GrowBranch(currentBranch);
	}
}

void ASpaceColonizationPlant::GrowBranch(FBranch* ToGrow) {
	if (ToGrow->ChildBranches.Num() >= MaxNumberOfBranchingTwigs) {
		GrowingBranches.Remove(ToGrow);
	}
	else {
		FVector normalizedGrowthDirection = ToGrow->GrowDirection;
		normalizedGrowthDirection = normalizedGrowthDirection.GetSafeNormal();

		if (ToGrow->GrowCount == 1 && ToGrow->ChildBranches.Num() < 1) {
			// Grow current branch farther out
			ToGrow->End = ToGrow->End + normalizedGrowthDirection * GrowthPerIteration;
		}
		else if (ToGrow->GrowCount > 0) {
			// create new Branch
			FBranch* newBranch = new FBranch();
			if (ToGrow->ChildBranches.Num() > 0) {
				newBranch->BranchDepth = ToGrow->BranchDepth + 1;
				
			}
			else {
				newBranch->BranchDepth = ToGrow->BranchDepth;
			}
			if (newBranch->BranchDepth <= MaxGrowthDepth) {
				ToGrow->ChildBranches.Add(newBranch);
				newBranch->ParentBranch = ToGrow;
				newBranch->Start = ToGrow->End;
				newBranch->End = newBranch->Start + normalizedGrowthDirection * GrowthPerIteration;
				GrowingBranches.Add(newBranch);
			}
		}
	}
	ToGrow->ResetForNextGrowthIteration();
}