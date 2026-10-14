// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "EStat.h"
#include "StatsComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_SPARSE_DELEGATE_OneParam(FOnHealthPercentUpdateSignature, UStatsComponent, OnHealthPercentUpdateDelegate, float, Percentage);

DECLARE_DYNAMIC_MULTICAST_SPARSE_DELEGATE_OneParam(FOnStaminaPercentUpdateSignature, UStatsComponent, OnStaminaPercentUpdateDelegate, float, Percentage);

DECLARE_DYNAMIC_MULTICAST_SPARSE_DELEGATE(FOnZeroHealthSignature, UStatsComponent, OnZeroHealthDelegate);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class ACTIONCOMBAT_API UStatsComponent : public UActorComponent
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere)
	bool bCanRegen = true;

	UPROPERTY(EditAnywhere)
	float StaminaDelayDuration = 2.0f;


public:	
	// Sets default values for this component's properties
	UStatsComponent();

	UPROPERTY(EditAnywhere)
	TMap<TEnumAsByte<EStat>, float> Stats;

	UPROPERTY(BlueprintAssignable)
	FOnHealthPercentUpdateSignature OnHealthPercentUpdateDelegate;

	UPROPERTY(BlueprintAssignable)
	FOnStaminaPercentUpdateSignature OnStaminaPercentUpdateDelegate;

	UPROPERTY(BlueprintAssignable)
	FOnZeroHealthSignature OnZeroHealthDelegate;


protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;


	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	double StaminaRegenRate = 10.0;


	UFUNCTION(BlueprintCallable)
	void ReduceHealth(float Amount, AActor* Opponent);


	UFUNCTION(BlueprintCallable)
	void ReduceStamina(float Amount);

	UFUNCTION(BlueprintCallable)
	void RegenStamina();

	UFUNCTION()
	void EnableRegen();

	UFUNCTION(BlueprintPure)
	float GetStatPercentage(EStat Current, EStat Max);


	// 
	UFUNCTION(BlueprintCallable)
	void IncreaseStrength(float Amount);

	UFUNCTION(BlueprintCallable)
	void SetStrength(float Amount);

	UFUNCTION(BlueprintCallable)
	float GetStrength();

	UFUNCTION(BlueprintCallable)
	void SetMaxHealth(float Amount);

	UFUNCTION(BlueprintCallable)
	void AddHealth(float Amount);

	UFUNCTION(BlueprintCallable)
	void RegenHealth();


	UFUNCTION(BlueprintCallable)
	void SetMaxStamina(float Amount);


	///// Dot Damage Logic
	UFUNCTION(BlueprintCallable)
	void GetSmallDamage(float Amount);

	UPROPERTY(EditAnywhere,BlueprintReadWrite, Category="Dot")
	bool bDOTActive = false;

	UPROPERTY(EditAnywhere,BlueprintReadWrite, Category="Dot",  meta=(ClampMin="0.01"))
	float DOTInterval = 1.0f;

	UPROPERTY(EditAnywhere,BlueprintReadWrite, Category="Dot")
	float DOTDamagePerTick = 2.0f;

	UFUNCTION(BlueprintCallable, Category="DOT")
	void StartDOT();

	UFUNCTION(BlueprintCallable, Category="DOT")
	void StopDOT();


	FTimerHandle DotTimerHandle;
	void ApplyDOT_Interval();



	class UMainGameInstance* GameInstance;


};
