#include "Bachelor.h"
#include "MeshDataConstructor.h"
#include "KismetProceduralMeshLibrary.h"

UMeshDataConstructor::UMeshDataConstructor(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	
}
void UMeshDataConstructor::GenerateCylinder(FMeshData& MeshData, FVector BottomCenter,
	float BottomRadius, FVector TopCenter, float TopRadius, int NumSegments) {
	GenerateCircle(MeshData, BottomCenter, BottomRadius, 0.f, NumSegments);
	GenerateCircle(MeshData, TopCenter, TopRadius, 1.f, NumSegments);
	GenerateCylinderSectionTriangles(MeshData, BottomCenter, TopCenter, NumSegments, 0);
	UKismetProceduralMeshLibrary::CalculateTangentsForMesh(MeshData.Vertices, MeshData.Triangles, 
		MeshData.UVs, MeshData.Normals, MeshData.Tangents);
}

void UMeshDataConstructor::GenerateMultiLevelCylinder(FMeshData& MeshData, TArray<FVector> RingCenters,
	TArray<float> RingRadii, int NumSegmentsPerCircle) {
	int StartingIndex = MeshData.Vertices.Num();

	float TexCoordVIncrement = 1.0f / (RingCenters.Num() - 1);
	GenerateCircle(MeshData, RingCenters[0], RingRadii[0], 0.f, NumSegmentsPerCircle);
	for (int i = 1; i < RingCenters.Num(); ++i) {
		int TriangleIndexOffset = (i - 1) * (NumSegmentsPerCircle + 1) + StartingIndex;

		GenerateCircle(MeshData, RingCenters[i], RingRadii[i], i * TexCoordVIncrement, NumSegmentsPerCircle);
		GenerateCylinderSectionTriangles(MeshData, RingCenters[i - 1], RingCenters[i],
			NumSegmentsPerCircle, TriangleIndexOffset);
	}
	UKismetProceduralMeshLibrary::CalculateTangentsForMesh(MeshData.Vertices, MeshData.Triangles, 
		MeshData.UVs, MeshData.Normals, MeshData.Tangents);
}

void UMeshDataConstructor::GenerateCircle(FMeshData& MeshData,
	FVector Center, float Radius, float TexCoordV, int NumSegments) {

	float anglePerSection = (PI * 2.0f) / NumSegments;

	for (int i = 0; i < NumSegments + 1; ++i) {

		float currentAngle = anglePerSection * i;
		float sin;
		float cos;
		FMath::SinCos(&sin, &cos, currentAngle);
		FVector unitCirclePosition = FVector(cos, sin, 0.0f);

		MeshData.Vertices.Add(Center + Radius * unitCirclePosition);

		MeshData.Normals.Add(unitCirclePosition);

		float calculatedTexCoordU = (float)i / (float)NumSegments;
		FVector2D calculatedUV = FVector2D(calculatedTexCoordU, TexCoordV);
		MeshData.UVs.Add(calculatedUV);
	}
}

void UMeshDataConstructor::GenerateCylinderSectionTriangles(FMeshData& MeshData, FVector BottomCenter,
	FVector TopCenter, int NumSegments, int TriangleIndexOffset) {
	int verticesPerCircle = NumSegments + 1;

	FVector bottomToCenter = TopCenter - BottomCenter;
	bool bSwapTriangles = bottomToCenter.Z < 0.f;

	// starting triangle construction at top circle
	int indexStart = verticesPerCircle + TriangleIndexOffset;
	int indexEnd = (verticesPerCircle * 2) + TriangleIndexOffset;
	for (int i = indexStart; i < indexEnd; ++i) {
		int triangleIndex0 = i + 1;
		int triangleIndex1 = i;
		int triangleIndex2 = i - verticesPerCircle + 1;
		int triangleIndex3 = i - verticesPerCircle;

		if (bSwapTriangles) {
			MeshData.AddTriangle(triangleIndex1, triangleIndex2, triangleIndex0);
			MeshData.AddTriangle(triangleIndex1, triangleIndex3, triangleIndex2);
		}
		else {
			MeshData.AddTriangle(triangleIndex0, triangleIndex2, triangleIndex1);
			MeshData.AddTriangle(triangleIndex2, triangleIndex3, triangleIndex1);
		}
	}
}