// Fill out your copyright notice in the Description page of Project Settings.


#include "BTT_RangeAttack.h"
#include "AIController.h"
#include "GameFramework/Character.h"
#include "Kismet/KismetMathLibrary.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "EEnemyState.h"
#include "Fighter.h"



EBTNodeResult::Type UBTT_RangeAttack::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{   
    ACharacter* CharacterRef = OwnerComp.GetAIOwner()->GetPawn<ACharacter>();

    if (!CharacterRef)
    {
        return EBTNodeResult::Failed;
    }







    float Distance = OwnerComp.GetBlackboardComponent()->GetValueAsFloat(TEXT("Distance"));


    if (Distance < MeleeRange)
    {
        OwnerComp.GetBlackboardComponent()->SetValueAsEnum(TEXT("CurrentState2"), EEnemyState::Melee);

        AbortTask(OwnerComp, NodeMemory);

        UE_LOG(LogTemp, Error, TEXT("Distance < MeleeRange"));

        return EBTNodeResult::Aborted;
    }

    APawn* PlayerRef = GetWorld()->GetFirstPlayerController()->GetPawn(); 

    AAIController* AIRef = OwnerComp.GetAIOwner();

    AIRef->SetFocus(PlayerRef);

    UE_LOG(LogTemp, Warning, TEXT("Ranged Attack"));
    CharacterRef->PlayAnimMontage(AnimMontage);

    /*
    double RandomValue = UKismetMathLibrary::RandomFloat();

    if (RandomValue > Threshold)
    {
        Threshold = 0.9;
        OwnerComp.GetBlackboardComponent()->SetValueAsEnum(TEXT("CurrentState2"), EEnemyState::Charge);
    }
    else
    {
        Threshold -= 0.1;
    }
    */

    return EBTNodeResult::Succeeded;
}
