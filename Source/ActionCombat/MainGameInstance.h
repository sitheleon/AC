// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "MainGameInstance.generated.h"

/**
 * 
 */
UCLASS()
class ACTIONCOMBAT_API UMainGameInstance : public UGameInstance
{
	GENERATED_BODY()

public:
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Game Instance")
	bool bIsBossInitiated = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Game Instance")
	bool bIsMegaAttack = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Game Instance")
	bool bIsTraceSuccess = false;


	// Dot Damage
	/*
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Game Instance")
	bool bIsDotDamageTriggered = false;
	*/
};
