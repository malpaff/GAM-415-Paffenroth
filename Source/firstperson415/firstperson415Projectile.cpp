// Copyright Epic Games, Inc. All Rights Reserved.

#include "firstperson415Projectile.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Components/SphereComponent.h"
#include "Components/DecalComponent.h"
#include "Kismet/GameplayStatics.h"

Afirstperson415Projectile::Afirstperson415Projectile() 
{
	// Use a sphere as a simple collision representation
	CollisionComp = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComp"));
	CollisionComp->InitSphereRadius(5.0f);
	CollisionComp->BodyInstance.SetCollisionProfileName("Projectile");
	CollisionComp->OnComponentHit.AddDynamic(this, &Afirstperson415Projectile::OnHit);		// set up a notification for when this component hits something blocking

	// Players can't walk on it
	CollisionComp->SetWalkableSlopeOverride(FWalkableSlopeOverride(WalkableSlope_Unwalkable, 0.f));
	CollisionComp->CanCharacterStepUpOn = ECB_No;

	// Create a mesh component
	ballMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Ball Mesh"));

	// Set as root component
	RootComponent = CollisionComp;

	// Attach mesh to collision component
	ballMesh->SetupAttachment(CollisionComp);

	// Load the sphere mesh asset and assign it to mesh component
	static ConstructorHelpers::FObjectFinder<UStaticMesh> SphereVisualAsset(TEXT("/Engine/BasicShapes/Sphere"));

	if (SphereVisualAsset.Succeeded())
	{
		ballMesh->SetStaticMesh(SphereVisualAsset.Object);
		ballMesh->SetRelativeLocation(FVector::ZeroVector);
		ballMesh->SetRelativeScale3D(FVector(0.3f)); // Scale as needed
	}

	// Use a ProjectileMovementComponent to govern this projectile's movement
	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileComp"));
	ProjectileMovement->UpdatedComponent = CollisionComp;
	ProjectileMovement->InitialSpeed = 3000.f;
	ProjectileMovement->MaxSpeed = 3000.f;
	ProjectileMovement->bRotationFollowsVelocity = true;
	ProjectileMovement->bShouldBounce = true;

	// Die after 3 seconds by default
	InitialLifeSpan = 3.0f;
}

void Afirstperson415Projectile::BeginPlay()
{
	Super::BeginPlay();
	// Generates a random color for the projectile
	randColor = FLinearColor(UKismetMathLibrary::RandomFloatInRange(0.f, 1.f), UKismetMathLibrary::RandomFloatInRange(0.f, 1.f), UKismetMathLibrary::RandomFloatInRange(0.f, 1.f), 1.f);
	UE_LOG(LogTemp, Warning, TEXT("Projectile RandColor: R=%f G=%f B=%f"), randColor.R, randColor.G, randColor.B);

	// DEBUG: Prints the generated color to the output log
	UE_LOG(LogTemp, Warning, TEXT("Projectile RandColor: R=%f G=%f B=%f"), randColor.R, randColor.G, randColor.B);

	// Creates a dynamic material instance from the projectile material
	dmiMat = UMaterialInstanceDynamic::Create(projMat, this);
	if (!dmiMat)
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to create dmi"));
		return;
	}

	// Assigns the dynamic material to the mesh
	ballMesh->SetMaterial(0, dmiMat);

	// Set the random color on the material parameter
	dmiMat->SetVectorParameterValue("ProjColor", randColor);
}

void Afirstperson415Projectile::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	// Only add impulse and destroy projectile if we hit a physics
	if ((OtherActor != nullptr) && (OtherActor != this) && (OtherComp != nullptr) && OtherComp->IsSimulatingPhysics())
	{
		OtherComp->AddImpulseAtLocation(GetVelocity() * 100.0f, GetActorLocation());

		Destroy();
	}

	if (OtherActor != nullptr)
	{
		// Choose a random frame for the decal
		float frameNum = UKismetMathLibrary::RandomFloatInRange(0.f, 3.f);

		if (baseMat)
		{
			// Spawns decal at hit point
			UDecalComponent* Decal = UGameplayStatics::SpawnDecalAtLocation(
				GetWorld(),
				baseMat,
				FVector(FMath::FRandRange(20.f, 40.f)),
				Hit.Location,
				Hit.Normal.Rotation(),
				5.f // Lifespan in seconds
			);

			if (Decal)
			{
				// Creates a dynamic material for the decal
				UMaterialInstanceDynamic* MatInstance = Decal->CreateDynamicMaterialInstance();
				if (MatInstance)
				{
					// Applies the projectile color and frame to decal
					MatInstance->SetVectorParameterValue("Color", randColor);
					MatInstance->SetScalarParameterValue("Frame", frameNum);
				}
				else
				{
					UE_LOG(LogTemp, Warning, TEXT("Decal DMI creation failed"));
				}
			}
			else
			{
				UE_LOG(LogTemp, Warning, TEXT("Decal spawn failed"));
			}
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("baseMat is null"));
		}
	}
}