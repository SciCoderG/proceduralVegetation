// Fill out your copyright notice in the Description page of Project Settings.

#include "Bachelor.h"
#include "SphericalColonizationSpace.h"


// Sets default values
ASphericalColonizationSpace::ASphericalColonizationSpace()
{
	ColonizationSphere = CreateDefaultSubobject<USphereComponent>(TEXT("ColonizationSphere"));
	ColonizationSphere->SetupAttachment(RootComponent);
	
	// initial values
	ColonizationRadius = 150.0f;
	NumberOfGenerationPoints = 2000;
	RandomSeed = -1;

	DrawDebugPoints = false;
	DebugPointColor = FColor(0, 0, 0);
	DebugPointSize = 5.0f;

	InitValues();
}

#if WITH_EDITOR  
void ASphericalColonizationSpace::PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent) {
	Super::PostEditChangeProperty(PropertyChangedEvent);
	FName PropertyName = (PropertyChangedEvent.Property != NULL) ? PropertyChangedEvent.Property->GetFName() : NAME_None;

	if ((PropertyName == GET_MEMBER_NAME_CHECKED(AColonizationSpace, DrawDebugPoints))) {
		ColonizationSphere->SetHiddenInGame(!DrawDebugPoints);
	}
	if ((PropertyName == GET_MEMBER_NAME_CHECKED(ASphericalColonizationSpace, ColonizationRadius)))
	{
		GenerateRandomColonizationPoints();
	}
}
#endif

void ASphericalColonizationSpace::GenerateRandomColonizationPoints() {
	ColonizationPoints.Reset();
	InitValues();
	float scaledSphereRadius = ColonizationSphere->GetScaledSphereRadius();
	
	if (RandomSeed > 0) {
		RandomStream.Initialize(RandomSeed);
	}
	else {
		RandomStream.GenerateNewSeed();
	}

	for (int i = 0; i < NumberOfGenerationPoints; ++i) {
		FVector RandomPoint = this->GetActorLocation() + RandomStream.RandRange(-scaledSphereRadius, scaledSphereRadius) * RandomStream.VRand();
		ColonizationPoints.Add(RandomPoint);
	}
}

void ASphericalColonizationSpace::InitValues() {
	ColonizationSphere->SetSphereRadius(ColonizationRadius, false);
	ColonizationSphere->SetRelativeLocation(FVector(0.f, 0.f, 0.f));
	if (DrawDebugPoints) {
		ColonizationSphere->SetHiddenInGame(false);
	}
}

float ASphericalColonizationSpace::GetMaxDistanceFromCenter() {
	return ColonizationSphere->GetScaledSphereRadius();
}

