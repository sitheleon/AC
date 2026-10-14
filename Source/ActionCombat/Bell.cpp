// Fill out your copyright notice in the Description page of Project Settings.


#include "Bell.h"
#include "Components/StaticMeshComponent.h"

// Sets default values
ABell::ABell()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	BellMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BellMesh"));

}

// Called when the game starts or when spawned
void ABell::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ABell::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	ElapsedTime += DeltaTime;

	//float YawAngle = 50.0f * FMath::Sin(ElapsedTime * 2.0f);
	float PitchAngle = 50.0f * FMath::Sin(ElapsedTime * 2.0f);
	FRotator NewRotation = FRotator( PitchAngle, 0.0f, 0.0f);
	BellMesh->SetRelativeRotation(NewRotation); 

}

