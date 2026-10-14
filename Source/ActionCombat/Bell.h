// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Bell.generated.h"

UCLASS()
class ACTIONCOMBAT_API ABell : public AActor
{
	GENERATED_BODY()
	

private:
	float ElapsedTime = 0.0f;


public:	
	// Sets default values for this actor's properties
	ABell();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(EditAnywhere)
	class UStaticMeshComponent* BellMesh;


};
