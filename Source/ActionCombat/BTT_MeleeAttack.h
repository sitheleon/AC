// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"

#include "BTT_MeleeAttack.generated.h"


/**
 * 
 */
UCLASS()
class ACTIONCOMBAT_API UBTT_MeleeAttack : public UBTTaskNode
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	float AttackRadius = 500.0f;

	UPROPERTY(EditAnywhere)
	float AcceptableRadius = 100.0f;

	FScriptDelegate MoveDelegate; //store function in a variable

	bool bIsFinished = false;

	UPROPERTY(EditAnywhere)
	float MeleeRange = 1250.0f;

	UPROPERTY(EditAnywhere)
	float AttackRange;


protected:
	EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

	virtual void TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;

public:

	UBTT_MeleeAttack(); //constructor

	UFUNCTION()
	void FinishAttack();
	
};
