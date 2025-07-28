// Fill out your copyright notice in the Description page of Project Settings.


#include "PerlinProcTerrain.h"
#include "ProceduralMeshComponent.h"
#include "KismetProceduralMeshLibrary.h"

// Sets default values
APerlinProcTerrain::APerlinProcTerrain()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Procedural Mesh Component
	ProcMesh = CreateDefaultSubobject<UProceduralMeshComponent>("Procedural Mesh");
	RootComponent = ProcMesh;
}

// Called when the game starts or when spawned
void APerlinProcTerrain::BeginPlay()
{
	Super::BeginPlay();
	
	// Generates vertices and triangles
	CreateVertices();
	CreateTriangles();

	// Temporary array for normals and tangents
	TArray<FVector> TempNormals;
	TArray<FProcMeshTangent> TempTangents;
	UKismetProceduralMeshLibrary::CalculateTangentsForMesh(Vertices, Triangles, UV0, TempNormals, TempTangents);

	// Creates the mesh
	ProcMesh->CreateMeshSection(sectionID, Vertices, Triangles, TempNormals, UV0, UpVertexColors, TempTangents, true);
	
	// Material assignment
	if (Mat)
	{
		ProcMesh->SetMaterial(0, Mat);
	}
}

// Called every frame
void APerlinProcTerrain::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Deforms terrain around an impact point
void APerlinProcTerrain::AlterMesh(FVector impactPoint)
{

	FVector tempVector = impactPoint - this->GetActorLocation();

	for (int i = 0; i < Vertices.Num(); i++)
	{
		 

		if (FVector(Vertices[i] - tempVector).Size() < radius)
		{
			Vertices[i] = Vertices[i] - Depth;
		}
	}

	// Recalculate normals and tangents
	TArray<FVector> TempNormals;
	TArray<FProcMeshTangent> TempTangents;
	UKismetProceduralMeshLibrary::CalculateTangentsForMesh(Vertices, Triangles, UV0, TempNormals, TempTangents);

	// Updates mesh
	ProcMesh->UpdateMeshSection(sectionID, Vertices, TempNormals, UV0, UpVertexColors, TempTangents);
}

// Creates a grid of vertices
void APerlinProcTerrain::CreateVertices()
{
	for (int X = 0; X <= XSize; X++)
	{
		for (int Y = 0; Y <= YSize; Y++)
		{
			float Z = FMath::PerlinNoise2D(FVector2D(X * NoiseScale + 0.1, Y * NoiseScale + 0.1)) * ZMultiplier;
			GEngine->AddOnScreenDebugMessage(-1, 999.0f, FColor::Yellow, FString::Printf(TEXT("Z %f"), Z));
			Vertices.Add(FVector(X * Scale, Y * Scale, Z));
			UV0.Add(FVector2D(X * UVScale, Y * UVScale));
		}
	}
}

// Creates triangles
void APerlinProcTerrain::CreateTriangles()
{
	int Width = YSize + 1;
	int Vertex = 0;

	for (int X = 0; X < XSize; X++)
	{
		for (int Y = 0; Y < YSize; Y++)
		{
			Triangles.Add(Vertex);
			Triangles.Add(Vertex + 1);
			Triangles.Add(Vertex + Width);

			Triangles.Add(Vertex + 1);
			Triangles.Add(Vertex + Width + 1);
			Triangles.Add(Vertex + Width);

			Vertex++;
		}
		Vertex++;
	}
}

