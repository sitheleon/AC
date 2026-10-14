// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "CombatComponent.generated.h"




DECLARE_DYNAMIC_MULTICAST_SPARSE_DELEGATE_OneParam(FOnAttackPerformedSignature, UCombatComponent, OnAttackPerformedDelegate, float, Amount); //Event
UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class ACTIONCOMBAT_API UCombatComponent : public UActorComponent
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere)
	TArray<UAnimMontage*> AttackAnimations;

	ACharacter* CharacterRef;

	
	UPROPERTY(VisibleAnywhere)
	bool bCanAttack = true;

	UPROPERTY(EditAnywhere)
	float StaminaCost = 5.0f;

public:	
	// Sets default values for this component's properties
	UCombatComponent();

	UFUNCTION(BlueprintCallable)
	void ComboAttack();

	UPROPERTY(BlueprintAssignable)
	// Even Variable : public always
	FOnAttackPerformedSignature OnAttackPerformedDelegate;

	float AnimDuration;

	
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
	int32 ComboCounter = 0;

	//
	UPROPERTY(EditAnywhere)
	class UAnimMontage* SpecialAttackAnimation;

	UFUNCTION(BlueprintCallable)
	void PerformSpecialAttack();


protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION(BlueprintCallable)
	void HandleResetAttack();

	void RandomAttack();
		
};
