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

	KillDistance = 100.0f;
	RadiusOfInfluence = 500.0f;
	GrowthPerIteration = 10.0f;
	Tropism = FVector(0.0f, 0.0f, 1.0f);
	MaxNumGrowthIterations = 20;
	MaxNumberOfBranchingTwigs = 4;
	MaxGrowthDepth = 6;
	WeightedGrowth = true;

	SmoothOutBranchingAngles = true;

	MaxNumberOfNotDidNotGrowNums = 2;

	IsStillGrowing = true;

	AllMeshData = new FMeshData();

	CurrentColonizationPointCount = 0;

	TreeStructureWasGenerated = false;

	InitUtilityValues();


}

ASpaceColonizationPlant::~ASpaceColonizationPlant() {
	delete AllMeshData;
}

// Called when the game starts or when spawned
DECLARE_CYCLE_STAT(TEXT("SpaceColonizationPlant ~ BeginPlay"), STAT_BeginPlay, STATGROUP_SpaceColonization);
DECLARE_CYCLE_STAT(TEXT("SpaceColonizationPlant ~ SmoothOutBranchingAngles"), STAT_SmoothOutBranchingAngles, STATGROUP_SpaceColonization);
void ASpaceColonizationPlant::BeginPlay()
{
	SCOPE_CYCLE_COUNTER(STAT_BeginPlay);
	Super::BeginPlay();

	if (0 < GrowthSpaces.Num()) {
		GenerateTreeStructure();
	}
}

// Called every frame
void ASpaceColonizationPlant::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );
}

#if WITH_EDITOR  
void ASpaceColonizationPlant::PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent) {

	InitUtilityValues();
	FName PropertyName = (PropertyChangedEvent.Property != NULL) ? PropertyChangedEvent.Property->GetFName() : NAME_None;

	if ((PropertyName == GET_MEMBER_NAME_CHECKED(ASpaceColonizationPlant, MaxNumGrowthIterations))) {
		// Potential place to regenerate tree
	}
	
	Super::PostEditChangeProperty(PropertyChangedEvent);
}
#endif

void ASpaceColonizationPlant::GenerateTreeStructure() {
	
	if (!TreeStructureWasGenerated) {
		if (GrowthSpaces[0]->GetColonizationPoints().Num() < 1) {
			for (AColonizationSpace* currentSpace : GrowthSpaces) {
				currentSpace->GenerateRandomColonizationPoints();
			}
		}

		UE_LOG(LogTemp, Warning, TEXT("Name: %s"), *this->GetName());
		InitUtilityValues();
		ColonizeGivenSpaces();
		if (SmoothOutBranchingAngles) {
			SCOPE_CYCLE_COUNTER(STAT_SmoothOutBranchingAngles);
			UBranchUtility::SmoothOutBranchingAngles(RootBranch);
		}

		UMeshConstructor::GenerateTreeMesh(&TreeConstructionData);
		UE_LOG(LogTemp, Warning, TEXT("-----------------"));
		if (NULL != RootBranch) {
			UBranchUtility::RecursiveDeleteAllBranches(RootBranch);
		}
		TreeStructureWasGenerated = true;
	}
}


void ASpaceColonizationPlant::InitUtilityValues() {
	KillDistanceSquared = KillDistance * KillDistance;
	RadiusOfInfluenceSquared = RadiusOfInfluence * RadiusOfInfluence;

	ActorLocation = this->GetActorLocation();

	TreeConstructionData.Mesh = Mesh;
	TreeConstructionData.AllMeshData = AllMeshData;
	TreeConstructionData.RootBranch = RootBranch;

	if (Tropism.Size() > 1.0f) {
		Tropism = Tropism.GetSafeNormal() * 0.9f;
	}
}

DECLARE_CYCLE_STAT(TEXT("SpaceColonizationPlant ~ ColonizeGivenSpaces"), STAT_ColonizeGivenSpaces, STATGROUP_SpaceColonization);
void ASpaceColonizationPlant::ColonizeGivenSpaces() {
	SCOPE_CYCLE_COUNTER(STAT_ColonizeGivenSpaces);
	if (RootBranch != NULL) {
		UBranchUtility::RecursiveDeleteAllBranches(RootBranch);
	}
	RootBranch = new FBranch();
	TreeConstructionData.RootBranch = RootBranch;
	InitialRootGrowth();
	int iterations = 0;
	for (int i = 0; i < MaxNumGrowthIterations; ++i) {
		GrowthIteration();
		iterations = i+1;
		if (GrowingBranches.Num() < 1) {
			UE_LOG(LogTemp, Warning, TEXT("No Growing Branches left, interrupting Colonization"));
			break;
		}
		if (!IsStillGrowing) {
			UE_LOG(LogTemp, Warning, TEXT("No Colonization Points reachable, interrupting Colonization"));
			break;
		}
		if (CurrentColonizationPointCount < 1) {
			UE_LOG(LogTemp, Warning, TEXT("No Colonization Points left, interrupting Colonization"));
			break;
		}
	}
	UE_LOG(LogTemp, Warning, TEXT("Finished Space Colonization after %d Iterations with %d growing Branches left"), iterations, GrowingBranches.Num());
}

DECLARE_CYCLE_STAT(TEXT("SpaceColonizationPlant ~ InitialRootGrowth"), STAT_InitialRootGrowth, STATGROUP_SpaceColonization);
void ASpaceColonizationPlant::InitialRootGrowth() {
	SCOPE_CYCLE_COUNTER(STAT_InitialRootGrowth);

	AColonizationSpace* nearestCSpace = GetNearestColonizationSpace();

	RootBranch->Start = FVector(0.f);
	GrowingBranches.Add(RootBranch);
	if (NULL != nearestCSpace) {
		FVector plantToNearestCSpace = nearestCSpace->GetActorLocation() - ActorLocation;
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
		if (!IsStillGrowing) {
			UE_LOG(LogTemp, Warning, TEXT("RootGrowth: No Colonization Points reachable, interrupting Colonization"));
			break;
		}

		TArray<FBranch*> leafBranches = UBranchUtility::RecursiveGetAllLeaves(RootBranch);
		if (leafBranches.Num() != 1) {
			UE_LOG(LogTemp, Error, TEXT("RootGrowth: %d Leaf-Branches. Should be exactly 1."), leafBranches.Num());
		}
		FBranch* leaf = leafBranches[0];

		FVector leafEndToCSCenter = CSpaceLocation - (leaf->End + ActorLocation);
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

	FVector plantPosition = ActorLocation;
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

DECLARE_CYCLE_STAT(TEXT("SpaceColonizationPlant ~ GrowthIteration"), STAT_GrowthIteration, STATGROUP_SpaceColonization);
void ASpaceColonizationPlant::GrowthIteration() {
	SCOPE_CYCLE_COUNTER(STAT_GrowthIteration);
	CheckAllColonizationPoints();
	if (IsStillGrowing) {
		GrowAllBranches();
	}
}
DECLARE_CYCLE_STAT(TEXT("SpaceColonizationPlant ~ CheckAllColonizationPoints"), STAT_CheckAllColonizationPoints, STATGROUP_SpaceColonization);
bool ASpaceColonizationPlant::CheckAllColonizationPoints() {
	SCOPE_CYCLE_COUNTER(STAT_CheckAllColonizationPoints);

	IsStillGrowing = false;
	CurrentColonizationPointCount = 0;

	for (AColonizationSpace* currentSpace : GrowthSpaces) {
		for (FVector currentPoint : currentSpace->GetColonizationPoints()) {
			IsStillGrowing |= CheckColonizationPoint(&currentPoint);
			CurrentColonizationPointCount++;
		}
	}

	return IsStillGrowing;
}

DECLARE_CYCLE_STAT(TEXT("SpaceColonizationPlant ~ CheckColonizationPoint - first half"), STAT_CheckColonizationPointFirst, STATGROUP_SpaceColonization);
DECLARE_CYCLE_STAT(TEXT("SpaceColonizationPlant ~ CheckColonizationPoint - second half"), STAT_CheckColonizationPointSecond, STATGROUP_SpaceColonization);

bool ASpaceColonizationPlant::CheckColonizationPoint(FVector* ColonizationPoint) {
	// i don't want to copy this array around, so this function will be kinda big
	TArray<FBranch*> branchesInInfluenceRadius;
	{
		SCOPE_CYCLE_COUNTER(STAT_CheckColonizationPointFirst);
		for (FBranch* currentBranch : GrowingBranches) {
			FVector branchEndToPoint = (*ColonizationPoint) - (currentBranch->End + ActorLocation);
			float distancePointToBranchSquared = branchEndToPoint.SizeSquared();

			if (distancePointToBranchSquared < RadiusOfInfluenceSquared) {
				if (distancePointToBranchSquared < KillDistanceSquared) {
					RemoveFromGrowthSpaces(*ColonizationPoint);
					return false;
				}
				else {
					branchesInInfluenceRadius.Add(currentBranch);
				}
			}
		}
	}
	
	SCOPE_CYCLE_COUNTER(STAT_CheckColonizationPointSecond);

	if (branchesInInfluenceRadius.Num() < 1) {
		return false;
	}

	FBranch* nearestBranch = branchesInInfluenceRadius[0];

	FVector minDistanceVector = (*ColonizationPoint) - (nearestBranch->End + ActorLocation);
	float minDistanceSquared = minDistanceVector.SizeSquared();

	for (FBranch* currentBranch : branchesInInfluenceRadius) {
		FVector currentDistanceVector = (*ColonizationPoint) - (currentBranch->End + ActorLocation);
		float currentDistanceSquared = currentDistanceVector.SizeSquared();
		if (currentDistanceSquared < minDistanceSquared) {
			nearestBranch = currentBranch;
			minDistanceSquared = currentDistanceSquared;
			minDistanceVector = currentDistanceVector;
		}
	}

	nearestBranch->GrowCount++;
	nearestBranch->GrowDirection += minDistanceVector.GetSafeNormal();

	return true;
}

DECLARE_CYCLE_STAT(TEXT("SpaceColonizationPlant ~ RemoveFromGrowthSpaces"), STAT_RemoveFromGrowthSpaces, STATGROUP_SpaceColonization);
void ASpaceColonizationPlant::RemoveFromGrowthSpaces(FVector ToRemove) {
	SCOPE_CYCLE_COUNTER(STAT_RemoveFromGrowthSpaces);
	for (AColonizationSpace* currentSpace : GrowthSpaces) {
		currentSpace->GetColonizationPoints().Remove(ToRemove);
	}
}

DECLARE_CYCLE_STAT(TEXT("SpaceColonizationPlant ~ GrowAllBranches"), STAT_GrowAllBranches, STATGROUP_SpaceColonization);
void ASpaceColonizationPlant::GrowAllBranches() {
	SCOPE_CYCLE_COUNTER(STAT_GrowAllBranches);
	TSet<FBranch*> currentBranchesToGrow = GrowingBranches;
	for (FBranch* currentBranch : GrowingBranches) {
		GrowBranch(currentBranch);
	}
}

DECLARE_CYCLE_STAT(TEXT("SpaceColonizationPlant ~ GrowBranch"), STAT_GrowBranch, STATGROUP_SpaceColonization);
void ASpaceColonizationPlant::GrowBranch(FBranch* ToGrow) {
	SCOPE_CYCLE_COUNTER(STAT_GrowBranch);
	if (ToGrow->ChildBranches.Num() >= MaxNumberOfBranchingTwigs || (ToGrow->DidNotGrowCounter > MaxNumberOfNotDidNotGrowNums)) {
		GrowingBranches.Remove(ToGrow);
	}
	else {
		if (ToGrow->GrowCount > 0) {
			FVector normalizedGrowthDirection = ToGrow->GrowDirection;
			normalizedGrowthDirection = normalizedGrowthDirection.GetSafeNormal();
			normalizedGrowthDirection += Tropism;
			normalizedGrowthDirection = normalizedGrowthDirection.GetSafeNormal();
		
			float individualGrowthPerIteration = GrowthPerIteration;
			if (WeightedGrowth) {
				float depthWeight = ((MaxGrowthDepth + 1) - ToGrow->BranchDepth) / (MaxGrowthDepth + 1);
				individualGrowthPerIteration = GrowthPerIteration + (depthWeight * GrowthPerIteration);
			}
	
			TryCreatingNewBranch(ToGrow, normalizedGrowthDirection, individualGrowthPerIteration);
		}
		else {
			ToGrow->DidNotGrowCounter++;
		}
	}
	ToGrow-> GrowCount = 0;
	ToGrow->GrowDirection = FVector(0.f);
}

DECLARE_CYCLE_STAT(TEXT("SpaceColonizationPlant ~ TryCreatingNewBranch"), STAT_TryCreatingNewBranch, STATGROUP_SpaceColonization);
void ASpaceColonizationPlant::TryCreatingNewBranch(FBranch* Parent, FVector NormalizedGrowthDirection, float IndividualGrowthPerIteration) {
	SCOPE_CYCLE_COUNTER(STAT_TryCreatingNewBranch);
	FBranch* newBranch = new FBranch();
	newBranch->BranchDepth = Parent->BranchDepth;
	
	FVector parentNormal = (Parent->End - Parent->Start).GetSafeNormal();
	float absParentToGrowDirectionDot = FVector::DotProduct(parentNormal, NormalizedGrowthDirection);
	if (absParentToGrowDirectionDot < -0.99f) {
		Parent->DidNotGrowCounter++;
		return;
	}

	if (Parent->ChildBranches.Num() > 0) {
		newBranch->BranchDepth += 1;
	}
	if (newBranch->BranchDepth <= MaxGrowthDepth) {
		Parent->ChildBranches.Add(newBranch);
		newBranch->ParentBranch = Parent;
		newBranch->Start = Parent->End;
		newBranch->End = newBranch->Start + NormalizedGrowthDirection * IndividualGrowthPerIteration;
		GrowingBranches.Add(newBranch);

		Parent->DidNotGrowCounter = 0;
	}
	else {
		Parent->DidNotGrowCounter++;
	}
}
