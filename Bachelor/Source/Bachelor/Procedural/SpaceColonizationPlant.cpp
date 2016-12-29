// Fill out your copyright notice in the Description page of Project Settings.

#include "Bachelor.h"
#include "SpaceColonizationPlant.h"

#include "ProceduralMeshComponent.h"
#include "KismetProceduralMeshLibrary.h"

#include "ColonizationSpace.h"
#include "Branch.h"


// Sets default values
ASpaceColonizationPlant::ASpaceColonizationPlant()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	USphereComponent* Root = CreateDefaultSubobject<USphereComponent>(TEXT("RootComponent"));
	RootComponent = Root;

	Mesh = CreateDefaultSubobject<UProceduralMeshComponent>(TEXT("GeneratedMesh"));
	Mesh->SetupAttachment(RootComponent);

	RootBranch = new FBranch();
	
	KillDistance = 0.5f;

	RadiusOfInfluence = 10.f;

	GrowthPerIteration = 1.f;

	MaxNumGrowthIterations = 200.f;
}

ASpaceColonizationPlant::~ASpaceColonizationPlant() {
	delete RootBranch;
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

#if WITH_EDITOR
void ASpaceColonizationPlant::PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent) {
	if (GeneratedTreeMaterial) {
		Mesh->SetMaterial(0, GeneratedTreeMaterial);
	}

	ColonizeGivenSpaces();
	GenerateTreeMesh();

	Super::PostEditChangeProperty(PropertyChangedEvent);
}
#endif

void ASpaceColonizationPlant::ColonizeGivenSpaces() {

	InitialRootGrowth();
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
		RootBranch->End = (distPlantToNearestCSpace - RadiusOfInfluence / 2.0f - maxDistanceToCSpaceCenter) * normalPlantToNearestCSpace;
	}
	UE_LOG(LogTemp, Warning, TEXT("RootBranch End after initial Growth: %s"), *(RootBranch->End).ToString());
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

	}
	
}

void ASpaceColonizationPlant::GenerateTreeMesh() {
	TArray<FVector> boxVertices;
	TArray<int32> boxTriangles;
	TArray<FVector> boxNormals;
	TArray<FVector2D> boxUVs;
	TArray<FProcMeshTangent> boxTangents;

	FVector rootStartToEnd = (RootBranch->End) - (RootBranch->Start);
	FVector notRootStartToEnd = FVector(-rootStartToEnd.X, rootStartToEnd.Y, rootStartToEnd.Z);
	while (notRootStartToEnd == rootStartToEnd) {
		notRootStartToEnd.X -= 1.0f;
	}
	FVector orthoToRootStartToEnd1 = FVector::CrossProduct(rootStartToEnd, notRootStartToEnd);
	orthoToRootStartToEnd1 = orthoToRootStartToEnd1.GetSafeNormal();
	UE_LOG(LogTemp, Warning, TEXT("ortho1: %s"), *orthoToRootStartToEnd1.ToString());
	FVector orthoToRootStartToEnd2 = FVector::CrossProduct(rootStartToEnd, orthoToRootStartToEnd2);
	orthoToRootStartToEnd2 = orthoToRootStartToEnd2.GetSafeNormal();
	UE_LOG(LogTemp, Warning, TEXT("ortho2: %s"), *orthoToRootStartToEnd2.ToString());
	
	FVector diagonalStart = (RootBranch->Start) + 5.0f  * orthoToRootStartToEnd1 +5.0f * orthoToRootStartToEnd2;
	FVector diagonalEnd = (RootBranch->End) - 5.0f * orthoToRootStartToEnd1 - 5.0f * orthoToRootStartToEnd2;

	FVector boxDiagonal = diagonalEnd - diagonalStart;

	UE_LOG(LogTemp, Warning, TEXT("BoxDiagonal: %s"), *boxDiagonal.ToString());

	UKismetProceduralMeshLibrary::GenerateBoxMesh(boxDiagonal / 2.0f, boxVertices, boxTriangles, boxNormals, boxUVs, boxTangents);

	if (Mesh->GetNumSections() > 1) {
		Mesh->ClearAllMeshSections();
	}

	Mesh->CreateMeshSection(0, boxVertices, boxTriangles, boxNormals, boxUVs, TArray<FColor>(), boxTangents, false);
	Mesh->SetRelativeLocation(boxDiagonal / 2.0f);

}



