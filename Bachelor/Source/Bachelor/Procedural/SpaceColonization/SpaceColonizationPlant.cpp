// Fill out your copyright notice in the Description page of Project Settings.

#include "Bachelor.h"
#include "SpaceColonizationPlant.h"

#include "ProceduralMeshComponent.h"
#include "KismetProceduralMeshLibrary.h"

#include "ColonizationSpace.h"
#include "Bachelor/Procedural/Branch.h"
#include "Bachelor/Procedural/Data/MeshData.h"
#include "Bachelor/Procedural/Data/TreeConstructionData.h"
#include "Bachelor/Utility/MeshDataConstructor.h"
#include "Bachelor/Utility/MeshConstructor.h"
#include "Bachelor/Utility/BranchUtility.h"


// Sets default values
ASpaceColonizationPlant::ASpaceColonizationPlant()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	USphereComponent* Root = CreateDefaultSubobject<USphereComponent>(TEXT("RootComponent"));
	Root->InitSphereRadius(1.0f);
	RootComponent = Root;

	Mesh = CreateDefaultSubobject<UProceduralMeshComponent>(TEXT("GeneratedMesh"));
	Mesh->SetupAttachment(RootComponent);

	MinNumberOfSectionsPerBranch = 2;
	MaxNumberOfSectionsPerBranch = 12;
	KillDistance = 100.0f;
	RadiusOfInfluence = 500.0f;
	GrowthPerIteration = 10.0f;
	Tropism = FVector(0.0f, 0.0f, 1.0f);
	TrunkRadiusMultiplier = 1.41;
	MaxNumGrowthIterations = 20;
	MaxNumberOfBranchingTwigs = 4;
	MaxGrowthDepth = 6;
	WeightedGrowth = true;

	MaxNumberOfVerticesPerMeshSection = 3e5;
	BranchRadiusZero = 1.0f;
	BranchRadiusGrowthParameter = 2.0f;
	PolyReductionByCurveReduction = false;
	SmoothOutBranchingAngles = true;

	IsStillGrowing = true;

	AllMeshData = new FMeshData();
	TreeConstructionData = new FTreeConstructionData();

	InitUtilityValues();
}

ASpaceColonizationPlant::~ASpaceColonizationPlant() {
	delete AllMeshData;
	delete TreeConstructionData;
	delete RootBranch;
}

// Called when the game starts or when spawned
void ASpaceColonizationPlant::BeginPlay()
{
	Super::BeginPlay();

	UE_LOG(LogTemp, Warning, TEXT("Name: %s"), *this->GetName());
	InitUtilityValues();
	ColonizeGivenSpaces();
	if (SmoothOutBranchingAngles) {
		UBranchUtility::SmoothOutBranchingAngles(RootBranch);
	}
	if (PolyReductionByCurveReduction) {
		UBranchUtility::RecursiveReduceGrownBranches(RootBranch);
	}

	UMeshConstructor::GenerateTreeMesh(TreeConstructionData);
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

	if (NULL == TreeConstructionData) {
		TreeConstructionData = new FTreeConstructionData();
	}
	TreeConstructionData->Mesh = Mesh;
	TreeConstructionData->AllMeshData = AllMeshData;
	TreeConstructionData->RootBranch = RootBranch;

	TreeConstructionData->TrunkRadiusMultiplier = TrunkRadiusMultiplier;
	TreeConstructionData->MinNumberOfSectionsPerBranch = MinNumberOfSectionsPerBranch;
	TreeConstructionData->MaxNumberOfSectionsPerBranch = MaxNumberOfSectionsPerBranch;
	TreeConstructionData->MaxNumberOfVerticesPerMeshSection = MaxNumberOfVerticesPerMeshSection;
	TreeConstructionData->BranchRadiusZero = BranchRadiusZero;
	TreeConstructionData->BranchRadiusGrowthParameter = BranchRadiusGrowthParameter;
}

void ASpaceColonizationPlant::ColonizeGivenSpaces() {
	if (RootBranch != NULL) {
		UBranchUtility::RecursiveDeleteAllBranches(RootBranch);
	}
	RootBranch = new FBranch();
	TreeConstructionData->RootBranch = RootBranch;
	InitialRootGrowth();
	int iterations = 0;
	for (int i = 0; i < MaxNumGrowthIterations; ++i) {
		GrowthIteration();
		iterations = i+1;
		if (GrowingBranches.Num() < 1) {
			UE_LOG(LogTemp, Warning, TEXT("No Growing Branches left, interrupting Colonization"));
			break;
		}
		if (AllColonizationPoints.Num() < 1) {
			UE_LOG(LogTemp, Warning, TEXT("No Colonization Points left, interrupting Colonization"));
			break;
		}
		if (!IsStillGrowing) {
			UE_LOG(LogTemp, Warning, TEXT("No Colonization Points reachable, interrupting Colonization"));
			break;
		}
	}
	UE_LOG(LogTemp, Warning, TEXT("Finished Space Colonization after %d Iterations with %d growing Branches left"), iterations, GrowingBranches.Num());
}

void ASpaceColonizationPlant::InitialRootGrowth() {
	AColonizationSpace* nearestCSpace = GetNearestColonizationSpace();

	RootBranch->Start = FVector(0.f);
	GrowingBranches.Add(RootBranch);
	if (nearestCSpace) {
		FVector plantToNearestCSpace = nearestCSpace->GetActorLocation() - this->GetActorLocation();
		float distPlantToNearestCSpace = plantToNearestCSpace.Size();

		float maxDistanceToCSpaceCenter = nearestCSpace->GetMaxDistanceFromCenter();

		FVector normalPlantToNearestCSpace = plantToNearestCSpace.GetSafeNormal();
		if (distPlantToNearestCSpace > maxDistanceToCSpaceCenter) {
			// plant is outside of CSpace Center
			RootBranch->End = (distPlantToNearestCSpace - maxDistanceToCSpaceCenter) * normalPlantToNearestCSpace;	
		}
		else {
			RootBranch->End = plantToNearestCSpace.GetSafeNormal() * GrowthPerIteration;
		}

		bool PlantCanGrow = CheckAllColonizationPoints();
		for (int i = 0; i < MaxNumGrowthIterations; ++i) {
			if (PlantCanGrow) {
				return;
			}
			else {
				RootBranch->End += normalPlantToNearestCSpace * GrowthPerIteration;
			}
			PlantCanGrow |= CheckAllColonizationPoints();
		}
		UE_LOG(LogTemp, Warning, TEXT("RootGrowth: Exceeded Max Number of Growth Iterations and could not reach ColonizationSpace - interrupting Growth."));
	}
}

void ASpaceColonizationPlant::DoRootGrowthIterations(float MaxDistanceToCSpaceCenter, FVector CSpaceLocation) {
	float squaredMaxDistanceToCSCenter = MaxDistanceToCSpaceCenter * MaxDistanceToCSpaceCenter;

	for (int i = 0; i < MaxNumGrowthIterations; ++i) {
		GrowthIteration();
		if (GrowingBranches.Num() < 1) {
			UE_LOG(LogTemp, Warning, TEXT("RootGrowth: No Growing Branches left, interrupting Colonization"));
			break;
		}
		if (AllColonizationPoints.Num() < 1) {
			UE_LOG(LogTemp, Warning, TEXT("RootGrowth: No Colonization Points left, interrupting Colonization"));
			break;
		}
		if (!IsStillGrowing) {
			UE_LOG(LogTemp, Warning, TEXT("RootGrowth: No Colonization Points reachable, interrupting Colonization"));
			break;
		}

		TArray<FBranch*> leafBranches = UBranchUtility::RecursiveGetAllLeaves(RootBranch);
		if (leafBranches.Num() != 1) {
			UE_LOG(LogTemp, Error, TEXT("RootGrowth: %d Leaf-Branches. Should be exactly 1."), leafBranches.Num());
		}
		FBranch* leaf = leafBranches[0];

		FVector leafEndToCSCenter = CSpaceLocation - (leaf->End + this->GetActorLocation());
		float squaredLeafEndToCSCenter = leafEndToCSCenter.SizeSquared();
		if (squaredLeafEndToCSCenter < squaredMaxDistanceToCSCenter) {
			UE_LOG(LogTemp, Warning, TEXT("RootGrowth: Inside Colonization Space Radius - stopping Root Growth."));
			break;
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
	IsStillGrowing = CheckAllColonizationPoints();
	if (!IsStillGrowing) {
		return;
	}
	GrowAllBranches();
}

bool ASpaceColonizationPlant::CheckAllColonizationPoints() {
	TSet<FVector> allColonizationPoints = GetAllColonizationPoints();
	for (FVector currentPoint : allColonizationPoints) {
		CheckIfInKillZone(currentPoint);
	}
	allColonizationPoints = GetAllColonizationPoints();
	bool IsInfluencingGrowth = false;
	for (FVector currentPoint : allColonizationPoints) {
		IsInfluencingGrowth|= CheckColonizationPoint(currentPoint);
	}
	return IsInfluencingGrowth;
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

void ASpaceColonizationPlant::CheckIfInKillZone(FVector ColonizationPoint) {
	for (FBranch* currentBranch : GrowingBranches) {
		FVector branchEndToPoint = ColonizationPoint - (currentBranch->End + this->GetActorLocation());
		float distancePointToBranchSquared = branchEndToPoint.SizeSquared();
		if (distancePointToBranchSquared < KillDistanceSquared) {
			RemoveFromGrowthSpaces(ColonizationPoint);
			return;
		}
	}
}

bool ASpaceColonizationPlant::CheckColonizationPoint(FVector ColonizationPoint) {
	// i don't want to copy this array around, so this function will be kinda big
	TArray<FBranch*> branchesInInfluenceRadius;
	for (FBranch* currentBranch : GrowingBranches) {
		FVector branchEndToPoint = ColonizationPoint - (currentBranch->End + this->GetActorLocation());
		float distancePointToBranchSquared = branchEndToPoint.SizeSquared();

		if (distancePointToBranchSquared < RadiusOfInfluenceSquared) {
			branchesInInfluenceRadius.Add(currentBranch);
		}
	}

	bool IsInfluencingGrowth = false;
	if (branchesInInfluenceRadius.Num() < 1) {
		return IsInfluencingGrowth;
	}
	else {
		IsInfluencingGrowth = true;
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

	return IsInfluencingGrowth;
}

void ASpaceColonizationPlant::RemoveFromGrowthSpaces(FVector ToRemove) {
	for (AColonizationSpace* currentSpace : GrowthSpaces) {
		currentSpace->GetColonizationPoints()->Remove(ToRemove);
	}
}

void ASpaceColonizationPlant::GrowAllBranches() {
	TSet<FBranch*> currentBranchesToGrow = GrowingBranches;
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
		normalizedGrowthDirection += Tropism;
		normalizedGrowthDirection = normalizedGrowthDirection.GetSafeNormal();
		
		float individualGrowthPerIteration = GrowthPerIteration;
		if (WeightedGrowth) {
			float depthWeight = ((MaxGrowthDepth + 1) - ToGrow->BranchDepth) / (MaxGrowthDepth + 1);
			individualGrowthPerIteration = GrowthPerIteration + (depthWeight * GrowthPerIteration);
		}
	
		if (ToGrow->GrowCount > 0) {
			TryCreatingNewBranch(ToGrow, normalizedGrowthDirection, individualGrowthPerIteration);
		}
	}
	ToGrow-> GrowCount = 0;
	ToGrow->GrowDirection = FVector(0.f);
}

void ASpaceColonizationPlant::TryCreatingNewBranch(FBranch* Parent, FVector NormalizedGrowthDirection, float IndividualGrowthPerIteration) {
	FBranch* newBranch = new FBranch();
	newBranch->BranchDepth = Parent->BranchDepth;
	if (Parent->ChildBranches.Num() > 0) {
		newBranch->BranchDepth += 1;
	}
	if (newBranch->BranchDepth <= MaxGrowthDepth) {
		Parent->ChildBranches.Add(newBranch);
		newBranch->ParentBranch = Parent;
		newBranch->Start = Parent->End;
		newBranch->End = newBranch->Start + NormalizedGrowthDirection * IndividualGrowthPerIteration;
		GrowingBranches.Add(newBranch);
	}
}
