// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotifyState.h"
#include "MegaTraceState.generated.h"

/**
 * 
 */
UCLASS()
class ACTIONCOMBAT_API UMegaTraceState : public UAnimNotifyState
{
	GENERATED_BODY()
	


public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName SocketName = TEXT("Hand_L");

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float TraceLength = 200.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float TraceRadius = 10.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TEnumAsByte<ECollisionChannel> TraceChannel = ECollisionChannel::ECC_GameTraceChannel2;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bHeadshotOnly = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FName> HeadBoneNames { TEXT("head"), TEXT("neck_01") };

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bStopOnFirstValidHit = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bDrawDebug;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float DebugTime = 0.1f;


	virtual void NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration) override;
    virtual void NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float FrameDeltaTime) override;
    virtual void NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation) override;

	
	class UMainGameInstance* GameInstance;

private:

	bool DoTraceAndHandle(USkeletalMeshComponent* MeshComp);
    bool IsHeadshot(const FHitResult& Hit) const;


    bool bActive = false;
    bool bAlreadyHit = false;

	
};
