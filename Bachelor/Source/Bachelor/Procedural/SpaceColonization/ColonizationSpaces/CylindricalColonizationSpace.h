// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "Bachelor/Procedural/SpaceColonization/ColonizationSpace.h"
#include "CylindricalColonizationSpace.generated.h"

UCLASS()
class BACHELOR_API ACylindricalColonizationSpace : public AColonizationSpace
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ACylindricalColonizationSpace();

	virtual float GetMaxDistanceFromCenter() override;

	// Called on property changes
#if WITH_EDITOR  
	virtual void PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent);
#endif

	void InitCylinder(float Height, float Radius);

	virtual void GenerateRandomColonizationPoints() override;


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Procedural")
		float CylinderHeight;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Procedural")
		float CylinderRadius;

protected:


private:
	void InitValues();
};
