// BTT_MegaAttack.h
#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTT_MegaAttack.generated.h"

UCLASS()
class ACTIONCOMBAT_API UBTT_MegaAttack : public UBTTaskNode
{
    GENERATED_BODY()

public:
    UBTT_MegaAttack();

protected:
    virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
    virtual void OnTaskFinished(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, EBTNodeResult::Type TaskResult) override;

private:
    // 몽타주 종료 콜백
    UFUNCTION()
    void OnMontageEnded(UAnimMontage* Montage, bool bInterrupted);

    // 델리게이트/참조 정리
    void UnbindMontageEnd();

private:
    // BT 종료 호출을 위해 보관
    TWeakObjectPtr<UBehaviorTreeComponent> CachedOwnerComp;

    // 어떤 AnimInstance/몽타주에 바인딩했는지 추적
    TWeakObjectPtr<class UAnimInstance> BoundAnimInstance;
    TWeakObjectPtr<class UAnimMontage>  BoundMontage;

    // 대기 상태 플래그
    bool bWaitingMontage = false;
};
