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

	ColonizationSphere = CreateDefaultSubobject<USphereComponent>(TEXT("ColonizationSphere"));
	ColonizationSphere->SetupAttachment(RootComponent);
	
	// initial values
	ColonizationRadius = 150.0f;
	NumberOfGenerationPoints = 2000.0f;
	DrawDebugPoints = false;
	DebugPointColor = FColor(0, 0, 0);
	DebugPointSize = 5.0f;

	InitValues();
}

// Called when the game starts or when spawned
void AColonizationSpace::BeginPlay()
{
	Super::BeginPlay();
	
	InitValues();
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
	FName PropertyName = (PropertyChangedEvent.Property != NULL) ? PropertyChangedEvent.Property->GetFName() : NAME_None;

	if ((PropertyName == GET_MEMBER_NAME_CHECKED(AColonizationSpace, ColonizationRadius)) || 
		(PropertyName == GET_MEMBER_NAME_CHECKED(AColonizationSpace, NumberOfGenerationPoints)))
	{
		InitValues();
		GenerateRandomColonizationPoints();
		
	}
	else if ((PropertyName == GET_MEMBER_NAME_CHECKED(AColonizationSpace, DrawDebugPoints))) {
		ColonizationSphere->SetHiddenInGame(!DrawDebugPoints);
	}
	else {
		InitValues();
	}
}
#endif

TSet<FVector>* AColonizationSpace::GetColonizationPoints() {
	return &ColonizationPoints;
}

void AColonizationSpace::GenerateRandomColonizationPoints() {
	ColonizationPoints.Reset();
	float scaledSphereRadius = ColonizationSphere->GetScaledSphereRadius();
	for (uint32 i = 0; i < NumberOfGenerationPoints; ++i) {
		FVector RandomPoint = this->GetActorLocation() + FMath::RandRange(-scaledSphereRadius, scaledSphereRadius) * FMath::VRand();
		ColonizationPoints.Add(RandomPoint);
	}
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

void AColonizationSpace::InitValues() {
	ColonizationSphere->SetSphereRadius(ColonizationRadius, false);
	ColonizationSphere->SetRelativeLocation(FVector(0.f, 0.f, 0.f));
	if (DrawDebugPoints) {
		ColonizationSphere->SetHiddenInGame(false);
	}
}

float AColonizationSpace::GetMaxDistanceFromCenter() {
	return ColonizationSphere->GetScaledSphereRadius();
}

