// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "MainPlayer.h"
#include "Fighter.h"
#include "MainPlayer.h"

#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"

#include "MainCharacter.generated.h"


UCLASS()
class ACTIONCOMBAT_API AMainCharacter : public ACharacter, public IMainPlayer, public IFighter
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	class UAnimMontage* DeathAnimMontage;

	UPROPERTY(EditAnywhere)
	class UAnimMontage* HurtAnimMontage;


	//
	UPROPERTY(EditAnywhere)
	class UParticleSystem* HeadOutParticleTemplate;



public:
	// Sets default values for this character's properties
	AMainCharacter();

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	class UStatsComponent* StatsComp;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	class ULockonComponent* LockonComp;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	class UCombatComponent* CombatComp;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	class UTraceComponent* TraceComp;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	class UBlockComponent* BlockComp;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	class UPlayerActionsComponent* PlayerActionsComp;


	USpringArmComponent* SpringArmComp;
	UCameraComponent* CameraComp;

	//


	//
	float NormalArmLength = 300.0f;
	float SprintArmLength = 380.0f;
	float AttackArmLength = 240.0f;
	float CurrentTargetArmLength = 300.0f;
	
	float ZoomInterpSpeed = 8.0f;
	
	UPROPERTY(BlueprintReadWrite)
	bool bIsSprinting = false;

	UPROPERTY(BlueprintReadWrite)
	bool bIsAttacking = false;
	
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(BlueprintReadOnly)
	class UPlayerAnimInstance* PlayerAnim;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;


	virtual float GetDamage() override;


	// overriden from IMainPlayer (Interface Class)
	virtual bool HasEnoughStamina(float Cost) override;


	UFUNCTION(BlueprintCallable)
	void HandleDeath();


	virtual void EndLockonWithActor(AActor* ActorRef) override;

	virtual bool CanTakeDamage(AActor* Opponent) override;

	UFUNCTION(BlueprintCallable)
	void PlayHurtAnim(TSubclassOf<UCameraShakeBase>CameraShakeTemplate);


	UFUNCTION(BlueprintCallable)
	void playCameraShake(TSubclassOf<UCameraShakeBase> CameraShakeTemplate);

	UFUNCTION(BlueprintCallable)
	void SetCameraFocusOnTarget(AActor* TargetActor);

	FRotator OriginalControlRotation;
	FRotator OriginalCameraRotation;

	UFUNCTION(BlueprintCallable)
	void RestoreOriginalView();


	UFUNCTION(BlueprintCallable)
	void HideHeadBone();
	
	UFUNCTION()
	void GetInstantDeathDamage();



	///// Stats Gong

	//attack
	UFUNCTION(BlueprintCallable)
	void IncreaseDamage();

};
