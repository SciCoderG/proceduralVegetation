// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "ProceduralTest.generated.h"

class UProceduralMeshComponent;

UCLASS()
class BACHELOR_API AProceduralTest : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AProceduralTest();

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	// Called every frame
	virtual void Tick( float DeltaSeconds ) override;

	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Default)
	UMaterialInterface* GeneratedMeshMaterial;

private:

	UProceduralMeshComponent* mesh;
};
