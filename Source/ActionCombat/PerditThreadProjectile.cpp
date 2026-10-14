// Fill out your copyright notice in the Description page of Project Settings.


#include "PerditThreadProjectile.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
APerditThreadProjectile::APerditThreadProjectile()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;


	Collision = CreateDefaultSubobject<USphereComponent>(TEXT("Collision"));
	Collision->InitSphereRadius(8.f);
	Collision->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	Collision->SetCollisionObjectType(ECC_WorldDynamic);


	Collision->SetCollisionResponseToChannel(ECC_GameTraceChannel1, ECR_Block);

	Collision->SetNotifyRigidBodyCollision(true);

	RootComponent = Collision;

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	Mesh->SetupAttachment(RootComponent);
	Mesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	Movement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("Movement"));
	Movement->InitialSpeed = 2200.f;
	Movement->MaxSpeed = 2200.f;
	Movement->ProjectileGravityScale = 0.f;
	Movement->bRotationFollowsVelocity = true;
	Movement->bInitialVelocityInLocalSpace = false;

	InitialLifeSpan = 0.f;
}

// Called when the game starts or when spawned
void APerditThreadProjectile::BeginPlay()
{
	Super::BeginPlay();

	Collision->OnComponentHit.AddDynamic(this, &APerditThreadProjectile::OnHit);

	if (LifeSeconds > 0.f)
	{
		SetLifeSpan(LifeSeconds);
	}
	
}

void APerditThreadProjectile::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor,
				UPrimitiveComponent* OhterComp, FVector NormalImpulse, const FHitResult& Hit)
{
	if (OtherActor && OtherActor != GetOwner())
	{	
		UE_LOG (LogTemp, Error, TEXT("Hit"));
	
		UGameplayStatics::ApplyDamage(OtherActor, Damage, nullptr, GetOwner(), DamageType);
	}

	UE_LOG (LogTemp, Error, TEXT("NOT Hit"));
	
	Destroy();
}

// Called every frame
void APerditThreadProjectile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

