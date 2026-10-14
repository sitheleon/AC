// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Enemy.h"
#include "EEnemyState.h"
#include "Fighter.h"
#include "BossCharacter.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnEnemyDied);
UCLASS()
class ACTIONCOMBAT_API ABossCharacter : public ACharacter, public IEnemy, public IFighter //interface를 상속  
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	TEnumAsByte<EEnemyState> InitialState;

	class UBlackboardComponent* BlackboardComp;

	UPROPERTY(EditAnywhere)
	class UAnimMontage* DeathAnim;

	class AAIController* ControllerRef;


	//mega attack
	UPROPERTY(EditAnywhere)
	class UAnimMontage* MegaAttackAnim;



public:
	// Sets default values for this character's properties
	ABossCharacter();

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	class UStatsComponent* StatsComp;

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
	class UCombatComponent* CombatComp;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	class UTraceComponent* TraceComp;


	
	UPROPERTY(BlueprintAssignable, Category = "Event")
	FOnEnemyDied OnEnemyDied;

	class UMainGameInstance* GameInstance;


	//attach head to the hand logic
	UPROPERTY(EditDefaultsOnly)
	FName HandSocketName = TEXT("weapon_l_pos");


	UFUNCTION(BlueprintCallable)
	void ShowPlayerHeadBone();

	UPROPERTY(EditAnywhere, Category="Boss")
	class UStaticMesh* HeadStaticMesh; // 부착할 머리 Mesh

	UFUNCTION(BlueprintCallable)
	void StartRandomTimer();

	UFUNCTION(BlueprintCallable)
	void OnRandomTick();

	FTimerHandle RandomTimerHandle;


protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;




public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;


	UFUNCTION(BlueprintCallable)
	void DetectPawn(APawn* DetectedPawn, APawn* PawnToDetect);

	virtual float GetDamage() override;

	virtual void Attack() override;

	//IEnemy interface
	virtual void PerformMegaAttack() override;



	virtual float GetAnimDuration() override;


	UFUNCTION()
	void HandlePlayerDeath();
	

	UFUNCTION(BlueprintCallable)
	void HandleDeath();

	UFUNCTION(BlueprintCallable)
	void FinishDeathAnim();



	void FinishMegaAttack();


	UFUNCTION(BlueprintCallable)
	void SetToMegaAttack();
};
