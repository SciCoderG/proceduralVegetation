// Fill out your copyright notice in the Description page of Project Settings.

#include "Bachelor.h"
#include "CylindricalColonizationSpace.h"


// Sets default values
ACylindricalColonizationSpace::ACylindricalColonizationSpace()
{
	// initial values
	NumberOfGenerationPoints = 2000;
	RandomSeed = -1;

	DrawDebugPoints = false;
	DebugPointColor = FColor(0, 0, 0);
	DebugPointSize = 5.0f;

	CylinderHeight = 200.0f;
	CylinderRadius = 50.0f;

	InitValues();
}

#if WITH_EDITOR  
void ACylindricalColonizationSpace::PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent) {
	Super::PostEditChangeProperty(PropertyChangedEvent);
	FName PropertyName = (PropertyChangedEvent.Property != NULL) ? PropertyChangedEvent.Property->GetFName() : NAME_None;

	if ((PropertyName == GET_MEMBER_NAME_CHECKED(ACylindricalColonizationSpace, CylinderHeight)) ||
		(PropertyName == GET_MEMBER_NAME_CHECKED(ACylindricalColonizationSpace, CylinderRadius)))
	{
		GenerateRandomColonizationPoints();
	}
}
#endif

void ACylindricalColonizationSpace::InitCylinder(float Height, float Radius) {
	this->CylinderHeight = Height * GetActorScale3D().Z;
	this->CylinderRadius = Radius * FMath::Max(GetActorScale3D().X, GetActorScale3D().Y);
}


void ACylindricalColonizationSpace::GenerateRandomColonizationPoints() {
	ColonizationPoints.Reset();
	
	if (RandomSeed > 0) {
		RandomStream.Initialize(RandomSeed);
	}
	else {
		RandomStream.GenerateNewSeed();
	}

	for (int i = 0; i < NumberOfGenerationPoints; ++i) {
		FVector Random2DUnitVector = RandomStream.GetUnitVector();
		Random2DUnitVector = Random2DUnitVector.GetSafeNormal2D();
		FVector RandomRadialVector = Random2DUnitVector * RandomStream.FRandRange(-CylinderRadius, CylinderRadius);
		RandomRadialVector.Z = RandomStream.FRandRange(0.f, CylinderHeight);
		FVector RandomPoint = this->GetActorLocation() + RandomRadialVector;
		ColonizationPoints.Add(RandomPoint);
	}
}

void ACylindricalColonizationSpace::InitValues() {
	InitCylinder(CylinderHeight, CylinderRadius);
}

float ACylindricalColonizationSpace::GetMaxDistanceFromCenter() {
	return FMath::Sqrt(CylinderHeight * CylinderHeight + CylinderRadius * CylinderRadius); // a^2 + b^2 = c^2
}

