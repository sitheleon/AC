// Fill out your copyright notice in the Description page of Project Settings.


#include "LookAtPlayerAnimNotifyState.h"
#include "LookAtPlayerComponent.h"

void ULookAtPlayerAnimNotifyState::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration)
{
    AActor* OwnerRef = MeshComp->GetOwner();

    if (!IsValid(OwnerRef))
    {
        return;
    }

    ULookAtPlayerComponent* RotationComp = OwnerRef->FindComponentByClass<ULookAtPlayerComponent>();

    if (!IsValid(RotationComp))
    {
        return;
    }

    RotationComp->bCanRotate = true;

}

void ULookAtPlayerAnimNotifyState::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
    ULookAtPlayerComponent* RotationComp = MeshComp->GetOwner()->FindComponentByClass<ULookAtPlayerComponent>();

    if(!IsValid(RotationComp))
    {
        return;
    }

    RotationComp->bCanRotate = false;
}
