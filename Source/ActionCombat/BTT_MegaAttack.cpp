// BTT_MegaAttack.cpp
#include "BTT_MegaAttack.h"
#include "AIController.h"
#include "Enemy.h"          // IEnemy 인터페이스(혹은 캐릭터 클래스)
#include "MainCharacter.h"  // 필요시
#include "Animation/AnimInstance.h"
#include "GameFramework/Character.h"

UBTT_MegaAttack::UBTT_MegaAttack()
{
    // 트레이스는 AnimNotifyState가 하므로 Tick 불필요
    bNotifyTick = false;
}

EBTNodeResult::Type UBTT_MegaAttack::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
    CachedOwnerComp = &OwnerComp;

    AAIController* AI = OwnerComp.GetAIOwner();
    if (!AI) return EBTNodeResult::Failed;

    ACharacter* Char = Cast<ACharacter>(AI->GetPawn());
    if (!Char) return EBTNodeResult::Failed;

    UAnimInstance* Anim = Char->GetMesh() ? Char->GetMesh()->GetAnimInstance() : nullptr;
    if (!Anim) return EBTNodeResult::Failed;

    // 캐릭터 이동 막기
    Char->GetCharacterMovement()->DisableMovement();

    UAnimMontage* PlayedMontage = nullptr;

    if (IEnemy* Enemy = Cast<IEnemy>(Char))
    {
        Enemy->PerformMegaAttack();
        PlayedMontage = Anim->GetCurrentActiveMontage();
    }

    if (!PlayedMontage)
    {
        // 이동 허용 복구
        Char->GetCharacterMovement()->SetMovementMode(MOVE_Walking);
        return EBTNodeResult::Failed;
    }

    {
        FOnMontageEnded EndDelegate;
        EndDelegate.BindUObject(this, &UBTT_MegaAttack::OnMontageEnded);
        Anim->Montage_SetEndDelegate(EndDelegate, PlayedMontage);
    }

    BoundAnimInstance = Anim;
    BoundMontage      = PlayedMontage;
    bWaitingMontage   = true;

    return EBTNodeResult::InProgress;
}

void UBTT_MegaAttack::OnMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{
    if (BoundMontage.Get() != Montage) return;

    UBehaviorTreeComponent* BTC = CachedOwnerComp.Get();
    if (!BTC) { UnbindMontageEnd(); return; }

    // 캐릭터 이동 다시 허용
    if (BTC->GetAIOwner())
    {
        if (ACharacter* Char = Cast<ACharacter>(BTC->GetAIOwner()->GetPawn()))
        {
            Char->GetCharacterMovement()->SetMovementMode(MOVE_Walking);
        }
    }

    bWaitingMontage = false;
    UnbindMontageEnd();

    FinishLatentTask(*BTC, bInterrupted ? EBTNodeResult::Failed : EBTNodeResult::Succeeded);
}

void UBTT_MegaAttack::OnTaskFinished(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, EBTNodeResult::Type)
{
    // 태스크가 어떤 이유로든 끝나면 델리게이트 해제
    UnbindMontageEnd();
}

void UBTT_MegaAttack::UnbindMontageEnd()
{
    if (UAnimInstance* Anim = BoundAnimInstance.Get())
    {
        if (UAnimMontage* Montage = BoundMontage.Get())
        {
            // 빈 델리게이트로 덮어써서 해제
            FOnMontageEnded Empty;
            Anim->Montage_SetEndDelegate(Empty, Montage);
    BoundMontage      = nullptr;
}   }
    BoundAnimInstance = nullptr;
    BoundMontage      = nullptr;
}
