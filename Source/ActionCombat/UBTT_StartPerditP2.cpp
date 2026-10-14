// Fill out your copyright notice in the Description page of Project Settings.


#include "UBTT_StartPerditP2.h"
#include "AIController.h"
#include "PerditBossComponent.h"


UUBTT_StartPerditP2::UUBTT_StartPerditP2()
{
    bNotifyTick = true;
}

EBTNodeResult::Type UUBTT_StartPerditP2::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
    if (AAIController* AI = OwnerComp.GetAIOwner())
    {
        if (APawn* Pawn = AI->GetPawn())
        {
            if (auto* Comp = Pawn->FindComponentByClass<UPerditBossComponent>())
            {
                Comp->StartPhase2Loop();

                return EBTNodeResult::InProgress;
            }
        }
    }

    return EBTNodeResult::Failed;
}

void UUBTT_StartPerditP2::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
    if (AAIController* AI = OwnerComp.GetAIOwner())
    {
        if (APawn* Pawn = AI->GetPawn())
        {
            if (!Pawn || Pawn->IsPendingKill() /*Not yet */ || Pawn->IsActorBeingDestroyed())
            {
                FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
                return;
            }


            //
        }
    }
}

EBTNodeResult::Type UUBTT_StartPerditP2::AbortTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
    if (AAIController* AI = OwnerComp.GetAIOwner())
    {
        if (APawn* Pawn = AI->GetPawn())
        {
            if (auto* Comp = Pawn->FindComponentByClass<UPerditBossComponent>())
            {
                Comp->StopPhase2Loop();
            }
        }
    }


    return EBTNodeResult::Aborted;
}
