// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "Bachelor/Procedural/SpaceColonization/ColonizationSpace.h"
#include "SphericalColonizationSpace.generated.h"

UCLASS()
class BACHELOR_API ASphericalColonizationSpace : public AColonizationSpace
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASphericalColonizationSpace();

	virtual float GetMaxDistanceFromCenter();

	// Called on property changes
	virtual void PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Procedural")
	USphereComponent* ColonizationSphere;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Procedural")
	float ColonizationRadius;

protected:

	virtual void GenerateRandomColonizationPoints();

private:
	void InitValues();

};
