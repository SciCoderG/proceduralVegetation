// Fill out your copyright notice in the Description page of Project Settings.

#include "Bachelor.h"
#include "ProceduralTest.h"
#include "ProceduralMeshComponent.h"
#include "KismetProceduralMeshLibrary.h"


// Sets default values
AProceduralTest::AProceduralTest()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;


	USphereComponent* SphereComponent = CreateDefaultSubobject<USphereComponent>(TEXT("RootComponent"));
	RootComponent = SphereComponent;

	mesh = CreateDefaultSubobject<UProceduralMeshComponent>(TEXT("GeneratedMesh"));
	/**
	*	Create/replace a section for this procedural mesh component.
	*	@param	SectionIndex		Index of the section to create or replace.
	*	@param	Vertices			Vertex buffer of all vertex positions to use for this mesh section.
	*	@param	Triangles			Index buffer indicating which vertices make up each triangle. Length must be a multiple of 3.
	*	@param	Normals				Optional array of normal vectors for each vertex. If supplied, must be same length as Vertices array.
	*	@param	UV0					Optional array of texture co-ordinates for each vertex. If supplied, must be same length as Vertices array.
	*	@param	VertexColors		Optional array of colors for each vertex. If supplied, must be same length as Vertices array.
	*	@param	Tangents			Optional array of tangent vector for each vertex. If supplied, must be same length as Vertices array.
	*	@param	bCreateCollision	Indicates whether collision should be created for this section. This adds significant cost.
	*/
	//UFUNCTION(BlueprintCallable, Category = "Components|ProceduralMesh", meta = (AutoCreateRefTerm = "Normals,UV0,VertexColors,Tangents"))
	//	void CreateMeshSection(int32 SectionIndex, const TArray<FVector>& Vertices, const TArray<int32>& Triangles, const TArray<FVector>& Normals,
	// const TArray<FVector2D>& UV0, const TArray<FColor>& VertexColors, const TArray<FProcMeshTangent>& Tangents, bool bCreateCollision);

	TArray<FVector> vertices;

	vertices.Add(FVector(0, 0, 0));
	vertices.Add(FVector(0, 300, 0));
	vertices.Add(FVector(0, 0, 300));
	vertices.Add(FVector(0, 300, 300));

	TArray<int32> Triangles;
	UKismetProceduralMeshLibrary::CreateGridMeshTriangles(2, 2, true, Triangles);


	mesh->CreateMeshSection(0, vertices, Triangles, TArray<FVector>(), TArray<FVector2D>(), TArray<FColor>(), TArray<FProcMeshTangent>(), true);

	TArray<FVector> boxVertices;
	TArray<int32> boxTriangles;
	TArray<FVector> boxNormals;
	TArray<FVector2D> boxUVs;
	TArray<FProcMeshTangent> boxTangents;
	UKismetProceduralMeshLibrary::GenerateBoxMesh(FVector(50, 10, 10), boxVertices, boxTriangles, boxNormals, boxUVs, boxTangents);
	mesh->CreateMeshSection(1, boxVertices, boxTriangles, boxNormals, boxUVs, TArray<FColor>(), boxTangents, true);

	mesh->SetRelativeScale3D(FVector(2, 1, 1));

	

	// With default options
	//mesh->CreateMeshSection(1, vertices, Triangles, TArray<FVector>(), TArray<FVector2D>(), TArray<FColor>(), TArray<FProcMeshTangent>(), false);
	mesh->SetupAttachment(RootComponent);

	mesh->SetMaterial(0, GeneratedMeshMaterial);
	mesh->SetMaterial(1, GeneratedMeshMaterial);
}

// Called when the game starts or when spawned
void AProceduralTest::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AProceduralTest::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );

}

