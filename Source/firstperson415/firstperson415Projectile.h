// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "firstperson415Projectile.generated.h"

class USphereComponent;
class UProjectileMovementComponent;

UCLASS(config=Game)
class Afirstperson415Projectile : public AActor
{
	GENERATED_BODY()

	/** Sphere collision component */
	UPROPERTY(VisibleDefaultsOnly, Category=Projectile)
	USphereComponent* CollisionComp;

	/** Projectile movement component */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Movement, meta = (AllowPrivateAccess = "true"))
	UProjectileMovementComponent* ProjectileMovement;

	// Projectile Mesh
	UPROPERTY(EditAnywhere)
		UStaticMeshComponent* ballMesh;

	// Material for splat decal
	UPROPERTY(EditAnywhere)
		UMaterial* baseMat;

	// Generated Color
	UPROPERTY()
		FLinearColor randColor;

	// Dynamic material instance for color of the projectile mesh
	UPROPERTY()
		UMaterialInstanceDynamic* dmiMat;

protected:
	// Material is assigned to the mesh before runtime
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Projectile")
		UMaterialInterface* projMat;

public:
	Afirstperson415Projectile();

protected:
	virtual void BeginPlay();

public:

	/** called when projectile hits something */
	UFUNCTION()
	void OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

	/** Returns CollisionComp subobject **/
	USphereComponent* GetCollisionComp() const { return CollisionComp; }
	/** Returns ProjectileMovement subobject **/
	UProjectileMovementComponent* GetProjectileMovement() const { return ProjectileMovement; }
};

