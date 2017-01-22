#pragma once

#include "UnrealMath.h"
#include "ProceduralMeshComponent.h"
#include "MeshData.generated.h"


USTRUCT()
struct BACHELOR_API FMeshData
{
	GENERATED_USTRUCT_BODY()
public:

	FMeshData();

	void GenerateTangents();

	void CreateSectionFromData(UProceduralMeshComponent& MeshComponent, int SectionIndex, bool CreateCollision);

	void AddTriangle(int Index0, int Index1, int Index2);

	void Reset();

	TArray<FVector> Vertices;
	TArray<int32> Triangles;
	TArray<FVector> Normals;
	TArray<FProcMeshTangent> Tangents;
	TArray<FVector2D> UVs;
};

