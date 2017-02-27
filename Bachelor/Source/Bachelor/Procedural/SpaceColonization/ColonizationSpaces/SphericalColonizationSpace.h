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

	virtual float GetMaxDistanceFromCenter() override;

	// Called on property changes
#if WITH_EDITOR  
	virtual void PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent);
#endif

	virtual void GenerateRandomColonizationPoints() override;


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Procedural")
	USphereComponent* ColonizationSphere;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Procedural")
	float ColonizationRadius;

protected:


private:
	void InitValues();

};
