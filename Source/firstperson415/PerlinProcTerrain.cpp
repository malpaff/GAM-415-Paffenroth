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
	ProcMesh->bUseAsyncCooking = true;
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

	// Collision Setup
	ProcMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	ProcMesh->SetCollisionObjectType(ECC_WorldStatic);
	ProcMesh->SetCollisionResponseToAllChannels(ECR_Block);
	ProcMesh->bUseComplexAsSimpleCollision = true;
	ProcMesh->SetNotifyRigidBodyCollision(true);
	ProcMesh->SetGenerateOverlapEvents(true);
	ProcMesh->bUseComplexAsSimpleCollision = true;

	// Creates the mesh
	ProcMesh->CreateMeshSection(sectionID, Vertices, Triangles, TempNormals, UV0, UpVertexColors, TempTangents, /*bCreateCollision=*/true);
	
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
void APerlinProcTerrain::AlterMesh(const FVector ImpactPoint)
{
	const FVector LocalHit = ProcMesh->GetComponentTransform().InverseTransformPosition(ImpactPoint);

	const float RadiusSq = radius * radius;

	for (int32 i = 0; i < Vertices.Num(); ++i)
	{
		if (FVector::DistSquared(Vertices[i], LocalHit) <= RadiusSq)
		{
			Vertices[i] -= Depth;
		}
	}

	// Recalculate normals and tangents
	TArray<FVector> TempNormals;
	TArray<FProcMeshTangent> TempTangents;
	UKismetProceduralMeshLibrary::CalculateTangentsForMesh(Vertices, Triangles, UV0, TempNormals, TempTangents);

	// Updates mesh
	ProcMesh->UpdateMeshSection(sectionID, Vertices, TempNormals, UV0, UpVertexColors, TempTangents);
	ProcMesh->RecreatePhysicsState();

	// Debug
	UE_LOG(LogTemp, Warning, TEXT("AlterMesh: LocalHit=%s radius=%.1f depth=%s"),
		*LocalHit.ToString(), radius, *Depth.ToString());
}

// Creates a grid of vertices
void APerlinProcTerrain::CreateVertices()
{
	for (int X = 0; X <= XSize; X++)
	{
		for (int Y = 0; Y <= YSize; Y++)
		{
			float Z = FMath::PerlinNoise2D(FVector2D(X * NoiseScale + 0.1, Y * NoiseScale + 0.1)) * ZMultiplier;
			Vertices.Add(FVector(X * Scale, Y * Scale, Z));
			UV0.Add(FVector2D((float)X / (float)XSize, (float)Y / (float)YSize) * UVScale);
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

