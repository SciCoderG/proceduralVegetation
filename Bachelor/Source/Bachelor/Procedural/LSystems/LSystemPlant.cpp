// Fill out your copyright notice in the Description page of Project Settings.

#include "Bachelor.h"
#include "LSystemPlant.h"

#include "ProceduralMeshComponent.h"
#include "KismetProceduralMeshLibrary.h"

#include "Bachelor/Procedural/Data/MeshData.h"
#include "Bachelor/Procedural/Branch.h"
#include "Bachelor/Utility/MeshDataConstructor.h"
#include "Bachelor/Utility/MeshConstructor.h"
#include "Bachelor/Utility/BranchUtility.h"


// Sets default values
ALSystemPlant::ALSystemPlant()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	USphereComponent* Root = CreateDefaultSubobject<USphereComponent>(TEXT("RootComponent"));
	Root->InitSphereRadius(1.0f);
	RootComponent = Root;

	Mesh = CreateDefaultSubobject<UProceduralMeshComponent>(TEXT("GeneratedMesh"));
	Mesh->SetupAttachment(RootComponent);

	InitUtilityValues();

}

ALSystemPlant::~ALSystemPlant() {
	delete AllMeshData;
	delete RootBranch;
}

// Called when the game starts or when spawned
void ALSystemPlant::BeginPlay()
{
	Super::BeginPlay();

	UE_LOG(LogTemp, Warning, TEXT("Name: %s"), *this->GetName());

	if (SmoothOutBranchingAngles) {
		UBranchUtility::SmoothOutBranchingAngles(RootBranch);
	}
	if (PolyReductionByCurveReduction) {
		UBranchUtility::RecursiveReduceGrownBranches(RootBranch);
	}

	UMeshConstructor::GenerateTreeMesh(&TreeConstructionData);
}

// Called every frame
void ALSystemPlant::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );
}

void ALSystemPlant::PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent) {

	FName PropertyName = (PropertyChangedEvent.Property != NULL) ? PropertyChangedEvent.Property->GetFName() : NAME_None;

	/*if ((PropertyName == GET_MEMBER_NAME_CHECKED(ALSystemPlant, MaxNumGrowthIterations))) {
		// Potential place to regenerate tree
	}*/
	
	Super::PostEditChangeProperty(PropertyChangedEvent);
}


void ALSystemPlant::InitUtilityValues() {
	TreeConstructionData.Mesh = Mesh;
	TreeConstructionData.AllMeshData = AllMeshData;
	TreeConstructionData.RootBranch = RootBranch;
}