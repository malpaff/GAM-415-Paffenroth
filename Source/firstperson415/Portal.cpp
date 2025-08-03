// Fill out your copyright notice in the Description page of Project Settings.


#include "Portal.h"
#include "firstperson415Character.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
APortal::APortal()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Create portal components
	mesh = CreateDefaultSubobject<UStaticMeshComponent>("Mesh");
	boxComp = CreateDefaultSubobject<UBoxComponent>("Box Comp");
	sceneCapture = CreateDefaultSubobject<USceneCaptureComponent2D>("Capture");
	rootArrow = CreateDefaultSubobject<UArrowComponent>("Root Arrow");

	// Set root and attach other components
	RootComponent = boxComp;
	mesh->SetupAttachment(boxComp);
	sceneCapture->SetupAttachment(mesh);
	rootArrow->SetupAttachment(RootComponent);

	// Disable collision responses for the portal mesh
	mesh->SetCollisionResponseToAllChannels(ECR_Ignore);
}

// Called when the game starts or when spawned
void APortal::BeginPlay()
{
	Super::BeginPlay();

	// Binds overlap event with the function
	boxComp->OnComponentBeginOverlap.AddDynamic(this, &APortal::OnOverlapBegin);

	// Hides mesh from being captured in screen capture
	mesh->SetHiddenInSceneCapture(true);

	// If a material is assigned, set the portal mesh
	if (mat)
	{
		mesh->SetMaterial(0, mat);
	}
	
}

// Called every frame
void APortal::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	// Continuously updates portal scene capture
	UpdatePortals();
}

// Triggered when another actor overlaps with this portal's box comp
void APortal::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	// Checks if the overlapping actor is the player
	Afirstperson415Character* playerChar = Cast<Afirstperson415Character>(OtherActor);

	if (playerChar)
	{
		if (OtherPortal)
		{
			if (!playerChar->isTeleporting)
			{
				// Teleport player to the other portal's location
				playerChar->isTeleporting = true;
				FVector loc = OtherPortal->rootArrow->GetComponentLocation();
				playerChar->SetActorLocation(loc);

				// Prevents the player from instantly teleporting back
				FTimerHandle TimerHandle;
				FTimerDelegate TimerDelegate;
				TimerDelegate.BindUFunction(this, "SetBool", playerChar);
				GetWorld()->GetTimerManager().SetTimer(TimerHandle, TimerDelegate, 1, false);
			}
		}
	}
}

// Resets flag so that player can teleport again
void APortal::SetBool(Afirstperson415Character* playerChar)
{
	if (playerChar)
	{
		playerChar->isTeleporting = false;
	}
}

void APortal::UpdatePortals()
{
	// Calculate offset between portals
	FVector Location = this->GetActorLocation() - OtherPortal->GetActorLocation();
	// Gets the player's current camera view
	FVector camLocation = UGameplayStatics::GetPlayerCameraManager(GetWorld(), 0)->GetTransformComponent()->GetComponentLocation();
	FRotator camRotation = UGameplayStatics::GetPlayerCameraManager(GetWorld(), 0)->GetTransformComponent()->GetComponentRotation();
	// Applies the location offset
	FVector CombinedLocation = camLocation + Location;
	// Sets the scene capture to that position and orientation
	sceneCapture->SetWorldLocationAndRotation(CombinedLocation, camRotation);
}

