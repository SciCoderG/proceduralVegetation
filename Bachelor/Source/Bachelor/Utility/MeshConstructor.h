#pragma once

#include "UnrealMath.h"
#include "Procedural/Data/MeshData.h"
#include "MeshConstructor.generated.h"


UCLASS()
class BACHELOR_API UMeshConstructor : public UBlueprintFunctionLibrary
{
	GENERATED_UCLASS_BODY()
public:

	UFUNCTION(BlueprintCallable, Category = "Components|ProceduralMesh")
	static void GenerateCylinder(FMeshData& MeshData, FVector BottomCenter, float BottomRadius, FVector TopCenter, float TopRadius, int NumSegments);

private:
	// Calculates Vertices, Normals and UVs for a Circle and ads them to MeshData. No TriangleCalculation!
	static void GenerateCircle(FMeshData& MeshData, FVector Center, float Radius, float TexCoordV, int NumSegments);

	// Calculates TriangleIndices for a Cylinder. MeshData needs to have at least enough Data for two Rings with NumSegments!
	static void GenerateCylinderSectionTriangles(FMeshData& MeshData, FVector BottomCenter, FVector TopCenter, int NumSegments);

};

