// Fill out your copyright notice in the Description page of Project Settings.

#include "Bachelor.h"
#include "ColonizationSpace.h"


// Sets default values
AColonizationSpace::AColonizationSpace()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	USphereComponent* RootSphere = CreateDefaultSubobject<USphereComponent>(TEXT("RootComponent"));
	RootComponent = RootSphere;
	
	// initial values
	NumberOfGenerationPoints = 2000;
	RandomSeed = -1;

	DrawDebugPoints = false;
	DebugPointColor = FColor(0, 0, 0);
	DebugPointSize = 5.0f;

}

// Called when the game starts or when spawned
void AColonizationSpace::BeginPlay()
{
	Super::BeginPlay();

	GenerateRandomColonizationPoints();
}

// Called every frame
void AColonizationSpace::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );

	if (DrawDebugPoints) {
		DrawDebugColonizationPoints();
	}
}

#if WITH_EDITOR  
void AColonizationSpace::PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent) {
	Super::PostEditChangeProperty(PropertyChangedEvent);

	FName PropertyName = (PropertyChangedEvent.Property != NULL) ? PropertyChangedEvent.Property->GetFName() : NAME_None;

	if ((PropertyName == GET_MEMBER_NAME_CHECKED(AColonizationSpace, DrawDebugPoints))) {
		DrawDebugColonizationPoints();
	}

	if ((PropertyName == GET_MEMBER_NAME_CHECKED(AColonizationSpace, NumberOfGenerationPoints)) ||
		(PropertyName == GET_MEMBER_NAME_CHECKED(AColonizationSpace, RandomSeed)))
	{
		GenerateRandomColonizationPoints();
	}
}
#endif

TSet<FVector>& AColonizationSpace::GetColonizationPoints() {
	return ColonizationPoints;
}


void AColonizationSpace::DrawDebugColonizationPoints() {
	for (FVector Point : ColonizationPoints) {
		DrawDebugPoint(
			GetWorld(),
			Point,
			DebugPointSize,
			DebugPointColor
		);
	}	
}


