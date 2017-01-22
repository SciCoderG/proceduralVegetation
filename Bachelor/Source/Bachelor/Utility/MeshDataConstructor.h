#pragma once

#include "UnrealMath.h"
#include "Procedural/Data/MeshData.h"
#include "MeshDataConstructor.generated.h"


UCLASS()
class BACHELOR_API UMeshDataConstructor : public UBlueprintFunctionLibrary
{
	GENERATED_UCLASS_BODY()
public:

	UFUNCTION(BlueprintCallable, Category = "Components|ProceduralMesh")
	static void GenerateCylinder(FMeshData& MeshData, FVector BottomCenter, FVector BottomNormal, float BottomRadius, FVector TopCenter, FVector TopNormal, float TopRadius, int NumSegments);

	UFUNCTION(BlueprintCallable, Category = "Components|ProceduralMesh")
	static void GenerateMultiLevelCylinder(FMeshData& MeshData, TArray<FVector> RingCenters, TArray<FVector> ConnectionNormals,
		TArray<float> RingRadii, int NumSegmentsPerCircle);

private:
	// Calculates Vertices, Normals and UVs for a Circle and ads them to MeshData. No TriangleCalculation!
	static void GenerateCircle(FMeshData& MeshData, FVector Center, FVector CircleNormal, float Radius, float TexCoordV, int NumSegments);

	// Calculates TriangleIndices for a Cylinder. MeshData needs to have at least enough Data for two Rings with NumSegments!
	static void GenerateCylinderSectionTriangles(FMeshData& MeshData, FVector BottomCenter, FVector TopCenter, int NumSegments, int TriangleIndexOffset, bool SwapTriangles = false);

};

