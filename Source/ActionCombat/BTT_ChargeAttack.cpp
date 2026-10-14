// Fill out your copyright notice in the Description page of Project Settings.


#include "BTT_ChargeAttack.h"
#include "AIController.h"
#include "GameFramework/Character.h"
#include "BossAnimInstance.h"

#include "BehaviorTree/BlackboardComponent.h"

#include "GameFramework/CharacterMovementComponent.h"

#include "EEnemyState.h"



EBTNodeResult::Type UBTT_ChargeAttack::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{   

    ControllerRef = OwnerComp.GetAIOwner();
    CharacterRef = ControllerRef->GetCharacter();
    BossAnim = Cast<UBossAnimInstance>(CharacterRef->GetMesh()->GetAnimInstance());

    BossAnim->bIsCharging = true;

    OwnerComp.GetBlackboardComponent()->SetValueAsBool(TEXT("IsReadyToCharge"), false);

    bIsFinished = false;

    return EBTNodeResult::InProgress; 
}

void UBTT_ChargeAttack::ChargeAtPlayer()
{
    APawn* PlayerRef = GetWorld()->GetFirstPlayerController()->GetPawn();

    FVector PlayerLocation = PlayerRef->GetActorLocation();

    FAIMoveRequest MoveRequest = PlayerLocation;
    MoveRequest.SetUsePathfinding(true);
    MoveRequest.SetAcceptanceRadius(AcceptanceRadius);

    ControllerRef->MoveTo(MoveRequest);



    ControllerRef->SetFocus(PlayerRef);

    ControllerRef->ReceiveMoveCompleted.AddUnique(MoveCompletedDelegate);

    OriginalWalkSpeed = CharacterRef->GetCharacterMovement()->MaxWalkSpeed;

    CharacterRef->GetCharacterMovement()->MaxWalkSpeed = ChargeWalkSpeed; //change speed to charge speed
}

void UBTT_ChargeAttack::HandleMoveCompleted()
{
    BossAnim->bIsCharging = false;

    FTimerHandle AttackTimerHandle;

    CharacterRef->GetWorldTimerManager().SetTimer(AttackTimerHandle, this, &UBTT_ChargeAttack::FinishAttack, 1.0f, false);

    CharacterRef->GetCharacterMovement()->MaxWalkSpeed = OriginalWalkSpeed; //reset speed to original
}

void UBTT_ChargeAttack::FinishAttack()
{
    bIsFinished = true;
}



// constructor
UBTT_ChargeAttack::UBTT_ChargeAttack()
{
    bCreateNodeInstance = true;

    bNotifyTick = true;

    MoveCompletedDelegate.BindUFunction(this, "HandleMoveCompleted");
}

void UBTT_ChargeAttack::TickTask(UBehaviorTreeComponent& OwnerComp,
        uint8* NodeMemory,
        float DeltaSeconds)
{
    bool bIsReadyToCharge = OwnerComp.GetBlackboardComponent()->GetValueAsBool(TEXT("IsReadyToCharge"));


    if (bIsReadyToCharge)
    {
        OwnerComp.GetBlackboardComponent()->SetValueAsBool(TEXT("IsReadyToCharge"), false);
        ChargeAtPlayer();
    }

    if (!bIsFinished)
    {
        return;
    }

    OwnerComp.GetBlackboardComponent()->SetValueAsEnum(TEXT("CurrentState2"), EEnemyState::Melee);

    ControllerRef->ReceiveMoveCompleted.Remove(MoveCompletedDelegate);

    FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);




}
