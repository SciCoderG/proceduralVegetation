#include "Bachelor.h"
#include "MeshDataConstructor.h"
#include "KismetProceduralMeshLibrary.h"

UMeshDataConstructor::UMeshDataConstructor(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	
}
void UMeshDataConstructor::GenerateCylinder(FMeshData& MeshData, FVector BottomCenter, FVector BottomNormal,
	float BottomRadius, FVector TopCenter, FVector TopNormal, float TopRadius, int NumSegments) {
	GenerateCircle(MeshData, BottomCenter, BottomNormal, BottomRadius, 0.f, NumSegments, 0.f);
	GenerateCircle(MeshData, TopCenter, TopNormal, TopRadius, 1.f, NumSegments, 0.f);
	GenerateCylinderSectionTriangles(MeshData, BottomCenter, TopCenter, NumSegments, 0);
	UKismetProceduralMeshLibrary::CalculateTangentsForMesh(MeshData.Vertices, MeshData.Triangles, 
		MeshData.UVs, MeshData.Normals, MeshData.Tangents);
}

DECLARE_CYCLE_STAT(TEXT("UMeshDataConstructor ~ GenerateMultiLevelCylinder"), STAT_GenerateMultiLevelCylinder, STATGROUP_UMeshDataConstructor);
void UMeshDataConstructor::GenerateMultiLevelCylinder(FMeshData& MeshData, FCylinderData& CylinderData) {
	SCOPE_CYCLE_COUNTER(STAT_GenerateMultiLevelCylinder);

	TArray<FVector> RingCenters = CylinderData.RingCenters;
	TArray<FVector> ConnectionNormals = CylinderData.ConnectionNormals;
	TArray<float> RingRadii = CylinderData.RingRadii;
	int NumSegmentsPerCircle = CylinderData.NumberOfMeshSections;
	float ZRotationAngle = CylinderData.LocalZRotationAngle;

	int StartingIndex = MeshData.Vertices.Num();
	float TexCoordVIncrement = 1.0f / (RingCenters.Num() - 1);
	GenerateCircle(MeshData, RingCenters[0], ConnectionNormals[0], RingRadii[0], 0.f, NumSegmentsPerCircle, ZRotationAngle);
	for (int i = 1; i < RingCenters.Num(); ++i) {
		int TriangleIndexOffset = (i - 1) * (NumSegmentsPerCircle + 1) + StartingIndex;

		GenerateCircle(MeshData, RingCenters[i], ConnectionNormals[i], RingRadii[i], i * TexCoordVIncrement, NumSegmentsPerCircle, ZRotationAngle);
		GenerateCylinderSectionTriangles(MeshData, RingCenters[i - 1], RingCenters[i],
			NumSegmentsPerCircle, TriangleIndexOffset);
	}
}

DECLARE_CYCLE_STAT(TEXT("UMeshDataConstructor ~ GenerateCircle"), STAT_GenerateCircle, STATGROUP_UMeshDataConstructor);
void UMeshDataConstructor::GenerateCircle(FMeshData& MeshData,
	FVector Center, FVector CircleNormal, float Radius, float TexCoordV, int NumSegments, float ZRotationAngle) {
	SCOPE_CYCLE_COUNTER(STAT_GenerateCircle);

	float anglePerSection = (PI * 2.0f) / NumSegments;

	FQuat unitCircleRotation = FQuat::FindBetweenNormals(FVector::UpVector, CircleNormal);

	for (int i = 0; i < NumSegments + 1; ++i) {

		float currentAngle = anglePerSection * i;
		currentAngle += ZRotationAngle;
		float sin;
		float cos;
		FMath::SinCos(&sin, &cos, currentAngle);
		FVector positionOnUnitCircle = FVector(cos, sin, 0.0f);
		positionOnUnitCircle = unitCircleRotation.RotateVector(positionOnUnitCircle);
		
		MeshData.Vertices.Add(Center + Radius * positionOnUnitCircle);

		MeshData.Normals.Add(positionOnUnitCircle);

		FVector tangentVec = FVector::CrossProduct(CircleNormal, positionOnUnitCircle);
		FProcMeshTangent tangent(tangentVec, false);
		MeshData.Tangents.Add(tangent);

		float calculatedTexCoordU = (float)i / (float)NumSegments;
		FVector2D calculatedUV = FVector2D(calculatedTexCoordU, TexCoordV);
		MeshData.UVs.Add(calculatedUV);
	}
}

DECLARE_CYCLE_STAT(TEXT("UMeshDataConstructor ~ GenerateCylinderSectionTriangles"), STAT_GenerateCylinderSectionTriangles, STATGROUP_UMeshDataConstructor);
void UMeshDataConstructor::GenerateCylinderSectionTriangles(FMeshData& MeshData, FVector BottomCenter,
	FVector TopCenter, int NumSegments, int TriangleIndexOffset, bool SwapTriangles) {
	SCOPE_CYCLE_COUNTER(STAT_GenerateCylinderSectionTriangles);

	int verticesPerCircle = NumSegments + 1;

	FVector bottomToCenter = TopCenter - BottomCenter;
	// starting triangle construction at top circle
	int indexStart = verticesPerCircle + TriangleIndexOffset;
	int indexEnd = (verticesPerCircle * 2) + TriangleIndexOffset;
	for (int i = indexStart; i < indexEnd; ++i) {
		int triangleIndex0 = i + 1;
		int triangleIndex1 = i;
		int triangleIndex2 = i - verticesPerCircle + 1;
		int triangleIndex3 = i - verticesPerCircle;

		if (SwapTriangles) {
			MeshData.AddTriangle(triangleIndex1, triangleIndex2, triangleIndex0);
			MeshData.AddTriangle(triangleIndex1, triangleIndex3, triangleIndex2);
		}
		else {
			MeshData.AddTriangle(triangleIndex0, triangleIndex2, triangleIndex1);
			MeshData.AddTriangle(triangleIndex2, triangleIndex3, triangleIndex1);
		}
	}
}