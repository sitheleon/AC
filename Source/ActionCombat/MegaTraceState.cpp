// Fill out your copyright notice in the Description page of Project Settings.

#include "MegaTraceState.h"

#include "DrawDebugHelpers.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/Character.h"
#include "MainCharacter.h"

#include "MainGameInstance.h"

void UMegaTraceState::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration)
{
    bActive = true;
    bAlreadyHit = false;

    //UE_LOG(LogTemp, Warning, TEXT("MegaTraceState: NotifyBegin - Animation started, Duration: %.2f"), TotalDuration);
    
    if (MeshComp && MeshComp->GetOwner())
    {
        //UE_LOG(LogTemp, Warning, TEXT("MegaTraceState: Owner: %s, Socket: %s"), 
               //*MeshComp->GetOwner()->GetName(), *SocketName.ToString());
    }
}

void UMegaTraceState::NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float FrameDeltaTime)
{
    if (!bActive || bAlreadyHit || !MeshComp)
    {   
        if (!bActive)
        {
            UE_LOG(LogTemp, Log, TEXT("MegaTraceState: NotifyTick skipped - not active"));
        }

        else if (bAlreadyHit)
        {
            UE_LOG(LogTemp, Log, TEXT("MegaTraceState: NotifyTick skipped - already hit"));
        }
        
        else if (!MeshComp)
        {
            UE_LOG(LogTemp, Error, TEXT("MegaTraceState: NotifyTick skipped - MeshComp is null"));
        }
        
        return;
    } 

    UE_LOG(LogTemp, VeryVerbose, TEXT("MegaTraceState: NotifyTick - DeltaTime: %.3f"), FrameDeltaTime);
        
    if (MeshComp->GetOwner())
    {
        const bool bValidHit = DoTraceAndHandle(MeshComp);

        if (bValidHit)
        {
            UE_LOG(LogTemp, Warning, TEXT("MegaTraceState: Valid hit detected!"));
            
            if (bStopOnFirstValidHit)
            {
                bAlreadyHit = true;
                UE_LOG(LogTemp, Warning, TEXT("MegaTraceState: Stopping on first valid hit"));
            }
        }
    }
}

void UMegaTraceState::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
    bActive = false;
    
    //UE_LOG(LogTemp, Warning, TEXT("MegaTraceState: NotifyEnd - Animation finished"));
    
    if (bAlreadyHit)
    {
        //UE_LOG(LogTemp, Warning, TEXT("MegaTraceState: Animation ended with successful hit"));
    }
    else
    {
        //UE_LOG(LogTemp, Warning, TEXT("MegaTraceState: Animation ended with no valid hits"));
    }
}

bool UMegaTraceState::DoTraceAndHandle(USkeletalMeshComponent* MeshComp)
{
    AActor* OwnerActor = MeshComp->GetOwner();

    if (!OwnerActor) 
    {
        UE_LOG(LogTemp, Error, TEXT("MegaTraceState: OwnerActor is null"));
        return false;
    }

    const FVector Start = MeshComp->GetSocketLocation(SocketName);
    const FRotator Rot  = MeshComp->GetSocketRotation(SocketName);
    const FVector End   = Start + Rot.Vector() * TraceLength;

    UE_LOG(LogTemp, VeryVerbose, TEXT("MegaTraceState: Trace from %s to %s (Length: %.1f)"), 
           *Start.ToString(), *End.ToString(), TraceLength);

    FHitResult Hit;
    FCollisionQueryParams Params(SCENE_QUERY_STAT(MegaTrace), /*bTraceComplex*/ true, OwnerActor);
    Params.AddIgnoredActor(OwnerActor);

    bool bHit = false;

    if (TraceRadius > 0.f)
    {
        UE_LOG(LogTemp, VeryVerbose, TEXT("MegaTraceState: Performing sphere trace with radius %.1f"), TraceRadius);
        
        // 스피어 트레이스
        bHit = OwnerActor->GetWorld()->SweepSingleByChannel(
            Hit, Start, End, FQuat::Identity, TraceChannel,
            FCollisionShape::MakeSphere(TraceRadius), Params);
    }
    else
    {
        UE_LOG(LogTemp, VeryVerbose, TEXT("MegaTraceState: Performing line trace"));
        
        // 라인트레이스
        bHit = OwnerActor->GetWorld()->LineTraceSingleByChannel(
            Hit, Start, End, TraceChannel, Params);
    }
//


    if (bDrawDebug)
    {
        if (TraceRadius > 0.f)
        {
            // 캡슐처럼 보이게 두 끝에 구체 찍기 + 선
            DrawDebugSphere(OwnerActor->GetWorld(), Start, TraceRadius, 12, FColor::White, false, DebugTime);
            DrawDebugSphere(OwnerActor->GetWorld(), End,   TraceRadius, 12, FColor::White, false, DebugTime);
            DrawDebugLine(OwnerActor->GetWorld(), Start, End, FColor::White, false, DebugTime, 0, 1.f);
        }
        else
        {
            DrawDebugLine(OwnerActor->GetWorld(), Start, End, FColor::White, false, DebugTime, 0, 1.f);
        }
    }

    if (!bHit) 
    {
        UE_LOG(LogTemp, VeryVerbose, TEXT("MegaTraceState: No hit detected"));
        return false;
    }

    UE_LOG(LogTemp, Log, TEXT("MegaTraceState: Hit detected - Actor: %s, Bone: %s, Location: %s"), 
           Hit.GetActor() ? *Hit.GetActor()->GetName() : TEXT("NULL"),
           *Hit.BoneName.ToString(),
           *Hit.ImpactPoint.ToString());

    const bool bHead = IsHeadshot(Hit);
    const FColor Dot = bHead ? FColor::Green : FColor::Red;

    UE_LOG(LogTemp, Warning, TEXT("MegaTraceState: %s - Target: %s"), 
        bHead ? TEXT("HEADSHOT DETECTED") : TEXT("BODY HIT"), 
        Hit.GetActor() ? *Hit.GetActor()->GetName() : TEXT("NULL"));

    if (bDrawDebug)
    {
        DrawDebugSphere(OwnerActor->GetWorld(), Hit.ImpactPoint, 10.f, 12, Dot, false, DebugTime);
    }

    // 타겟이 플레이어인지 확인 & 처리
    if (AMainCharacter* Player = Cast<AMainCharacter>(Hit.GetActor()))
    {
        UE_LOG(LogTemp, Warning, TEXT("MegaTraceState: Player hit confirmed - %s"), *Player->GetName());

        

            UE_LOG(LogTemp, Error, TEXT("MegaTraceState: %s %s - VALID HIT!"),
                bHead ? TEXT("HEADSHOT") : TEXT("HIT"),
                *Player->GetName());

            // 즉사 처리/데미지: 프로젝트 규칙에 맞게 교체
            // 예) Player->Die(); 또는 인터페이스로 데미지 전달
            // UGameplayStatics::ApplyDamage(Player, DamageAmount, /*EventInstigator*/nullptr, OwnerActor, nullptr);

            GameInstance = Cast<UMainGameInstance>(OwnerActor->GetWorld()->GetGameInstance());

            if (!GameInstance)
            {
                return false;
            }

            GameInstance->bIsTraceSuccess = true;

            Player->HideHeadBone();

            Player->GetInstantDeathDamage();

            UE_LOG(LogTemp, Error, TEXT("Hit Head End"));
            return true; // 유효 히트
    }
    else
    {
        UE_LOG(LogTemp, Log, TEXT("MegaTraceState: Hit actor is not a player"));
    }

    return false;
}

bool UMegaTraceState::IsHeadshot(const FHitResult& Hit) const
{   


    if (HeadBoneNames.Num() == 0) 
    {
        bool bIsHead = Hit.BoneName == FName("head");

        UE_LOG(LogTemp, VeryVerbose, TEXT("MegaTraceState: Default headshot check - Bone: %s, IsHead: %s"), 
               *Hit.BoneName.ToString(), bIsHead ? TEXT("TRUE") : TEXT("FALSE"));
            
        return bIsHead;
    }
    
    for (const FName& Bone : HeadBoneNames)
    {
        if (Hit.BoneName == Bone) 
        {
            UE_LOG(LogTemp, Log, TEXT("MegaTraceState: Headshot confirmed - Bone: %s matches %s"), 
                   *Hit.BoneName.ToString(), *Bone.ToString());
            return true;
        }
    }
    
    UE_LOG(LogTemp, VeryVerbose, TEXT("MegaTraceState: Not a headshot - Bone: %s not in headshot list"), 
           *Hit.BoneName.ToString());

    return false;
}


