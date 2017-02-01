#include "Bachelor.h"
#include "MeshData.h"
#include "KismetProceduralMeshLibrary.h"

FMeshData::FMeshData() {
}

void FMeshData::GenerateTangents() {
	UKismetProceduralMeshLibrary::CalculateTangentsForMesh(Vertices, Triangles, UVs, Normals, Tangents);
}

void FMeshData::CreateSectionFromData(UProceduralMeshComponent& MeshComponent, int SectionIndex, bool CreateCollision) {

	if (Normals.Num() != Vertices.Num()) {
		Normals.Reset();
	}
	if (UVs.Num() != Vertices.Num()) {
		UVs.Reset();
	}
	if (Tangents.Num() != Vertices.Num()) {
		Tangents.Num();
	}

	MeshComponent.CreateMeshSection(SectionIndex, Vertices, Triangles, Normals, UVs, TArray<FColor>(), Tangents, CreateCollision);
}

void FMeshData::AddTriangle(int Index0, int Index1, int Index2) {
	Triangles.Add(Index0);
	Triangles.Add(Index1);
	Triangles.Add(Index2);
}

void FMeshData::Reset() {
	Vertices.Reset();
	Triangles.Reset();
	Normals.Reset();
	Tangents.Reset();
	UVs.Reset();
}

