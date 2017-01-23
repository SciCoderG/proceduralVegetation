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
	NumberOfGenerationPoints = 2000.0f;
	RandomSeed = -1;

	DrawDebugPoints = false;
	DebugPointColor = FColor(0, 0, 0);
	DebugPointSize = 5.0f;

	InitValues();
}


void ASphericalColonizationSpace::PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent) {
	Super::PostEditChangeProperty(PropertyChangedEvent);
	FName PropertyName = (PropertyChangedEvent.Property != NULL) ? PropertyChangedEvent.Property->GetFName() : NAME_None;

	if ((PropertyName == GET_MEMBER_NAME_CHECKED(AColonizationSpace, DrawDebugPoints))) {
		ColonizationSphere->SetHiddenInGame(!DrawDebugPoints);
	}
}

void ASphericalColonizationSpace::GenerateRandomColonizationPoints() {
	ColonizationPoints.Reset();
	float scaledSphereRadius = ColonizationSphere->GetScaledSphereRadius();
	
	if (RandomSeed > 0) {
		RandomStream.Initialize(RandomSeed);
	}
	else {
		RandomStream.GenerateNewSeed();
	}

	for (uint32 i = 0; i < NumberOfGenerationPoints; ++i) {
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

