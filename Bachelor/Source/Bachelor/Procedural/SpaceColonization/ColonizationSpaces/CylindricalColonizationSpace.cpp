// Fill out your copyright notice in the Description page of Project Settings.

#include "Bachelor.h"
#include "CylindricalColonizationSpace.h"


// Sets default values
ACylindricalColonizationSpace::ACylindricalColonizationSpace()
{
	// initial values
	NumberOfGenerationPoints = 2000.0f;
	RandomSeed = -1;

	DrawDebugPoints = false;
	DebugPointColor = FColor(0, 0, 0);
	DebugPointSize = 5.0f;

	CylinderHeight = 200.0f;
	CylinderRadius = 50.0f;

	InitCylinder(CylinderHeight, CylinderRadius);

	InitValues();
}


void ACylindricalColonizationSpace::PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent) {
	Super::PostEditChangeProperty(PropertyChangedEvent);
	FName PropertyName = (PropertyChangedEvent.Property != NULL) ? PropertyChangedEvent.Property->GetFName() : NAME_None;

	if ((PropertyName == GET_MEMBER_NAME_CHECKED(AColonizationSpace, DrawDebugPoints))) {
		
	}
}

void ACylindricalColonizationSpace::InitCylinder(float Height, float Radius) {
	this->CylinderHeight = Height * GetActorScale3D().Z;
	this->CylinderRadius = Radius * FMath::Max(GetActorScale3D().X, GetActorScale3D().Y);
}


FVector ACylindricalColonizationSpace::Get2DUnitVector()
{
	FVector Result;
	float L;
	do
	{
		Result.X = RandomStream.GetFraction() * 2.f - 1.f;
		Result.Y = RandomStream.GetFraction() * 2.f - 1.f;
		L = Result.SizeSquared();
	} while (L > 1.f || L < KINDA_SMALL_NUMBER);

	return Result.GetUnsafeNormal();
}



void ACylindricalColonizationSpace::GenerateRandomColonizationPoints() {
	ColonizationPoints.Reset();
	
	if (RandomSeed > 0) {
		RandomStream.Initialize(RandomSeed);
	}
	else {
		RandomStream.GenerateNewSeed();
	}

	for (uint32 i = 0; i < NumberOfGenerationPoints; ++i) {
		FVector RandomRadialVector = FVector(RandomStream.GetFraction(), RandomStream.GetFraction(), 0.f);

		FVector RandomPoint = this->GetActorLocation() + RandomStream.RandRange(-CylinderRadius, CylinderRadius) ;
		ColonizationPoints.Add(RandomPoint);
	}
}

void ACylindricalColonizationSpace::InitValues() {

}

float ACylindricalColonizationSpace::GetMaxDistanceFromCenter() {
	return FMath::Max(CylinderHeight / 2.0f, CylinderRadius);
}

