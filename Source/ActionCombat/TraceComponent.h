// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "FTraceSockets.h"
#include "TraceComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_SPARSE_DELEGATE(FOnLinetracePerformedSignature, UTraceComponent, OnLinetracePerformedDelegate);


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class ACTIONCOMBAT_API UTraceComponent : public UActorComponent
{
	GENERATED_BODY()

	USkeletalMeshComponent* SkeletalComp;

	UPROPERTY(EditAnywhere, Category = "Trace")
	FName SkeletalMeshComponentName;

	
	UPROPERTY(EditAnywhere)
	TArray<FTraceSockets> Sockets;


	UPROPERTY(EditAnywhere)
	double BoxCollisionLength = 30.0;

	UPROPERTY(EditAnywhere)
	bool bDebugMode = false;


	TArray<AActor*> TargetsToIgnore;

	UPROPERTY(EditAnywhere)
	class UParticleSystem* HitParticleTemplate;

	//
	UPROPERTY(EditAnywhere)
	class UParticleSystem* SwordParticleTemplate;

	UPROPERTY(EditAnywhere)
	class UParticleSystem* ClangParticleTemplate;




public:	
	// Sets default values for this component's properties
	UTraceComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION(BlueprintCallable)
	void HandleResetAttack();

	UPROPERTY(VisibleAnywhere)
	bool bIsAttacking = false;

	// 이번 공격에서 이미 브로드캐스트했는지 추적
	bool bHasBroadcastedThisAttack = false;

	UFUNCTION(BlueprintCallable)
	void PlaySwordParticle();

	UFUNCTION(BlueprintCallable)
	void PlayClangParticle();

	//for boss
	UFUNCTION(BlueprintCallable)
	void EquipWeapon();


	//delegate

	UPROPERTY(BlueprintAssignable)
	FOnLinetracePerformedSignature OnLinetracePerformedDelegate;



	// Is Line Trace Succes
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Trace Status")
    bool bLineTraceHit = false;

	    // 마지막 히트한 액터 (옵션)
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Trace Status")
    AActor* LastHitActor = nullptr;

		
};
