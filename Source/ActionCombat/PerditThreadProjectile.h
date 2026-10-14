// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PerditThreadProjectile.generated.h"

UCLASS()
class ACTIONCOMBAT_API APerditThreadProjectile : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	APerditThreadProjectile();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;


	UFUNCTION()
	void OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor,
				UPrimitiveComponent* OhterComp, FVector NormalImpulse, const FHitResult& Hit);


public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	//comp
	UPROPERTY(VisibleAnywhere)
	class USphereComponent* Collision;
	
	UPROPERTY(VisibleAnywhere)
	class UStaticMeshComponent* Mesh;

	UPROPERTY(VisibleAnywhere)
	class UProjectileMovementComponent* Movement;

	//PARAMS
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="Thread")
	float Damage = 5.f;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="Thread")
	TSubclassOf<UDamageType> DamageType;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="Thread")
	float LifeSeconds = 4.f;

};
