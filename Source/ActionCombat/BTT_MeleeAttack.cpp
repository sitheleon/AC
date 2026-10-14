// Fill out your copyright notice in the Description page of Project Settings.


#include "BTT_MeleeAttack.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "AIController.h"
#include "Fighter.h"
#include "GameFramework/Character.h"
#include "EEnemyState.h"



EBTNodeResult::Type UBTT_MeleeAttack::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{   
    bIsFinished = false;

    float Distance = OwnerComp.GetBlackboardComponent()->GetValueAsFloat(TEXT("Distance"));

    AAIController* AIRef = OwnerComp.GetAIOwner();

    if (Distance > AttackRadius) // player too far from enemy
    {
        APawn* PlayerRef = GetWorld()->GetFirstPlayerController()->GetPawn(); 

        FVector PlayerLocation = PlayerRef->GetActorLocation();
        FVector AILocation = AIRef->GetPawn()->GetActorLocation();
        FVector DirectionToPlayer = (PlayerLocation - AILocation).GetSafeNormal();

        // 플레이어와 일정 거리(공격 범위의 80%)만큼 떨어진 목표 위치 계산
        float DesiredDistance = AttackRadius * 0.5f;
        FVector TargetLocation = PlayerLocation - (DirectionToPlayer * DesiredDistance);

        FAIMoveRequest MoveRequest;
        MoveRequest.SetGoalLocation(TargetLocation);
        MoveRequest.SetUsePathfinding(true);
        MoveRequest.SetAcceptanceRadius(AcceptableRadius);

        AIRef->MoveTo(MoveRequest);
        AIRef->SetFocus(PlayerRef);

        AIRef->ReceiveMoveCompleted.AddUnique(MoveDelegate);
    }
    else
    {   
        IFighter* FighterRef = Cast<IFighter>(AIRef->GetCharacter());
        FighterRef->Attack();

        FTimerHandle AttackTimer;
        AIRef->GetCharacter()->GetWorldTimerManager().SetTimer(AttackTimer, this, &UBTT_MeleeAttack::FinishAttack, FighterRef->GetAnimDuration(), false);
    }
    
    return EBTNodeResult::InProgress;
}

void UBTT_MeleeAttack::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
    float Distance = OwnerComp.GetBlackboardComponent()->GetValueAsFloat(TEXT("Distance"));
    AAIController* AIRef = OwnerComp.GetAIOwner();

    // 플레이어가 너무 멀어지면 Range 상태로 전환
    if (Distance > MeleeRange)
    {
        OwnerComp.GetBlackboardComponent()->SetValueAsEnum(TEXT("CurrentState2"), (uint8)EEnemyState::Range);

        FinishLatentTask(OwnerComp, EBTNodeResult::Aborted);

        AIRef->StopMovement();
        AIRef->ClearFocus(EAIFocusPriority::Gameplay);
        AIRef->ReceiveMoveCompleted.Remove(MoveDelegate);
        return;
    }

    // 플레이어가 너무 가까워지면 잠시 뒤로 이동
    if (Distance < AttackRadius * 0.5f && !bIsFinished)
    {   
        //log ok

        APawn* PlayerRef = GetWorld()->GetFirstPlayerController()->GetPawn();
        FVector PlayerLocation = PlayerRef->GetActorLocation();
        FVector AILocation = AIRef->GetPawn()->GetActorLocation();
        FVector AwayFromPlayer = (AILocation - PlayerLocation).GetSafeNormal();
        
        // 플레이어로부터 멀어지는 방향으로 이동
        FVector BackoffLocation = AILocation + (AwayFromPlayer * 100.0f);
        
        FAIMoveRequest BackoffRequest;
        BackoffRequest.SetGoalLocation(BackoffLocation);
        BackoffRequest.SetUsePathfinding(true);
        BackoffRequest.SetAcceptanceRadius(30.0f);
        

        AIRef->MoveTo(BackoffRequest);
    }

    if (!bIsFinished)
    {
        return;
    }
    
    // 공격 완료 후 정리
    OwnerComp.GetAIOwner()->ReceiveMoveCompleted.Remove(MoveDelegate);
    FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
}

UBTT_MeleeAttack::UBTT_MeleeAttack() // constructor
{
    MoveDelegate.BindUFunction(this, FName("FinishAttack"));
    
    bNotifyTick = true; // enable tick

    bCreateNodeInstance = true;
}

void UBTT_MeleeAttack::FinishAttack()
{
    bIsFinished = true;
}
