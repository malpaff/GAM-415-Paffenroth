// Copyright Epic Games, Inc. All Rights Reserved.

#include "firstperson415Projectile.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Components/SphereComponent.h"
#include "Components/DecalComponent.h"
#include "Kismet/GameplayStatics.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraComponent.h"
#include "PerlinProcTerrain.h"

Afirstperson415Projectile::Afirstperson415Projectile() 
{
	// Use a sphere as a simple collision representation
	CollisionComp = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComp"));
	CollisionComp->InitSphereRadius(3.0f);
	CollisionComp->BodyInstance.SetCollisionProfileName("Projectile");
	CollisionComp->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	CollisionComp->SetNotifyRigidBodyCollision(true);
	CollisionComp->OnComponentHit.AddDynamic(this, &Afirstperson415Projectile::OnHit);		// set up a notification for when this component hits something blocking
	CollisionComp->SetCollisionResponseToAllChannels(ECR_Block);
	CollisionComp->SetCollisionResponseToChannel(ECC_Pawn, ECR_Ignore);

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
static ConstructorHelpers::FObjectFinder<UStaticMesh> SphereVisualAsset(TEXT("/Engine/BasicShapes/Sphere.Sphere"));

if (SphereVisualAsset.Succeeded())
{
	ballMesh->SetStaticMesh(SphereVisualAsset.Object);
	ballMesh->SetRelativeLocation(FVector::ZeroVector);
	ballMesh->SetRelativeScale3D(FVector(0.3f));
}

// Use a ProjectileMovementComponent to govern this projectile's movement    // Only CollisionComp will handle collisions
ballMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileComp"));
ProjectileMovement->UpdatedComponent = CollisionComp;
ProjectileMovement->InitialSpeed = 3000.f;
ProjectileMovement->MaxSpeed = 3000.f;
ProjectileMovement->bRotationFollowsVelocity = true;
ProjectileMovement->bShouldBounce = true;
ProjectileMovement->OnProjectileStop.AddDynamic(this, &Afirstperson415Projectile::OnProjectileStop);

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
	if (!OtherActor || OtherActor == this)
	{
		Destroy();
		return;
	}

	// Spawn Niagara particle burst
	if (colorP)
	{
		UNiagaraComponent* ParticleComp = UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), colorP, Hit.Location, Hit.Normal.Rotation(), FVector(1.f), true, true, ENCPoolMethod::AutoRelease);

		if (ParticleComp)
		{
			// Send the projectile's random color into Niagara
			ParticleComp->SetVariableLinearColor(FName("RandomColor"), randColor);
		}
	}
	// Spawn Decal
	if (baseMat)
	{
		float frameNum = UKismetMathLibrary::RandomFloatInRange(0.f, 3.f);

		UDecalComponent* Decal = UGameplayStatics::SpawnDecalAtLocation(GetWorld(), baseMat, FVector(FMath::FRandRange(25.f, 40.f)), Hit.Location, Hit.Normal.Rotation(), 5.f);

		if (Decal)
		{
			UMaterialInstanceDynamic* MatInstance = Decal->CreateDynamicMaterialInstance();
			if (MatInstance)
			{
				MatInstance->SetVectorParameterValue("Color", randColor);
				MatInstance->SetScalarParameterValue("Frame", frameNum);
			}
		}
	}

	// If terrain is hit, alter the mesh
	if (APerlinProcTerrain* Terrain = Cast<APerlinProcTerrain>(OtherActor))
	{
		Terrain->AlterMesh(Hit.ImpactPoint);
	}
	else if (OtherComp)
	{
		if (APerlinProcTerrain* OwnerTerrain = Cast<APerlinProcTerrain>(OtherComp->GetOwner()))
		{
			OwnerTerrain->AlterMesh(Hit.ImpactPoint);
		}
	}
	// Destroy projectile
	Destroy();
}

// Fallback to deform terrain on contact
void Afirstperson415Projectile::OnProjectileStop(const FHitResult& Hit)
{
	if (APerlinProcTerrain* Terrain = Cast<APerlinProcTerrain>(Hit.GetActor()))
	{
		Terrain->AlterMesh(Hit.ImpactPoint);
	}
	else if (UPrimitiveComponent* Comp = Hit.GetComponent())
	{
		if (APerlinProcTerrain* OwnerTerrain = Cast<APerlinProcTerrain>(Comp->GetOwner()))
		{
			OwnerTerrain->AlterMesh(Hit.ImpactPoint);
		}
	}
	Destroy();
}
