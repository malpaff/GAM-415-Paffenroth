// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/SceneCaptureComponent2D.h"
#include "Engine/TextureRenderTarget2D.h"
#include "Components/BoxComponent.h"
#include "Components/ArrowComponent.h"
#include "Portal.generated.h"

class firstperson415Character;

UCLASS()
class FIRSTPERSON415_API APortal : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	APortal();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Portal Mesh
	UPROPERTY(EditAnywhere)
		UStaticMeshComponent* mesh;
	
	// Scene Capture component allowing us to view through the portal
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		USceneCaptureComponent2D* sceneCapture;

	// Arrow component for the portal's forward direction
	UPROPERTY(EditAnywhere)
		UArrowComponent* rootArrow;

	// Used to show the portal's view
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		UTextureRenderTarget2D* renderTarget;

	// Box collision component for interacting with the portal
	UPROPERTY(EditAnywhere)
		UBoxComponent* boxComp;

	// Reference to the paired portal
	UPROPERTY(EditAnywhere)
		APortal* OtherPortal;

	// Material assigned to the portal
	UPROPERTY(EditAnywhere)
		UMaterialInterface* mat;

	// Triggers teleportation logic
	UFUNCTION()
		void OnOverlapBegin(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	// Helper function to reset teleportation flag
	UFUNCTION()
	void SetBool(Afirstperson415Character* playerChar);

	// Updates the scene capture's position and rotation
	UFUNCTION()
		void UpdatePortals();
};
