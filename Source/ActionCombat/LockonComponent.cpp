// Fill out your copyright notice in the Description page of Project Settings.


#include "LockonComponent.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "GameFramework/SpringArmComponent.h"
#include "Math/Vector.h"
#include "Enemy.h"

// Sets default values for this component's properties
ULockonComponent::ULockonComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;


	// ...
}


// Called when the game starts
void ULockonComponent::BeginPlay()
{
	Super::BeginPlay();


	OwnerRef = GetOwner<ACharacter>();
	Controller = GetWorld()->GetFirstPlayerController();
	MovementComp = OwnerRef->GetCharacterMovement();


	SpringArmComp = OwnerRef->FindComponentByClass<USpringArmComponent>();

	// ...
	
}

void ULockonComponent::StartLockon(float Radius)
{	
	FHitResult OutResult;
	FVector CurrentLocation = OwnerRef-> GetActorLocation();
	FCollisionShape Sphere = FCollisionShape::MakeSphere(Radius);
	FCollisionQueryParams IgnoreParams { FName{ TEXT("Ignore Collision Params")}, false, OwnerRef};

	bool bHasFoundTarget = 
	
	GetWorld()->SweepSingleByChannel(
	OutResult,
	CurrentLocation,
	CurrentLocation,
	FQuat::Identity, // == object has an rotaion
	ECollisionChannel::ECC_GameTraceChannel1,
	Sphere,
	IgnoreParams
	);

	if(!bHasFoundTarget)
	{
		return;
	}

	if(!OutResult.GetActor()->Implements<UEnemy>()) // if has interface  UEnemy
	{
		return;
	}

	CurrentTargetActor = OutResult.GetActor();

	Controller->SetIgnoreLookInput(true);
	MovementComp->bOrientRotationToMovement = false;
	MovementComp->bUseControllerDesiredRotation = true; //smooth rotation

	SpringArmComp->TargetOffset = FVector {0.0, 0.0, 100.0};

	IEnemy::Execute_OnSelect(CurrentTargetActor); // From interface IEnemy, will be implemented on BP_Boss

	OnUpdatedTargetDelegate.Broadcast(CurrentTargetActor);


	UE_LOG(LogTemp, Warning, TEXT("Actor Detected: %s"), *OutResult.GetActor()->GetName());




}

void ULockonComponent::EndLockon()
{
	// 타겟이 유효한 경우에만 OnDeselect 호출
	if (IsValid(CurrentTargetActor))
	{
		IEnemy::Execute_OnDeselect(CurrentTargetActor); // From interface IEnemy, will be implemented on BP_Boss
	}

	CurrentTargetActor = nullptr;

	// 컴포넌트들이 유효한지 확인
	if (MovementComp)
	{
		MovementComp->bOrientRotationToMovement = true;
		MovementComp->bUseControllerDesiredRotation = false;
	}

	if (SpringArmComp)
	{
		SpringArmComp->TargetOffset = FVector::ZeroVector;
	}

	if (Controller)
	{
		Controller->ResetIgnoreLookInput();
	}

	OnUpdatedTargetDelegate.Broadcast(CurrentTargetActor);
}

void ULockonComponent::ToggleLockon(float Radius)
{
	if(IsValid(CurrentTargetActor)) // 이미 Locked 돼 있다는 뜻임
	{
		EndLockon();

	}
	else
	{

		StartLockon(Radius);
	}
}



// Called every frame
void ULockonComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (!IsValid(CurrentTargetActor))
	{
		// 타겟이 무효하면 락온 상태를 완전히 정리
		if (CurrentTargetActor != nullptr)
		{
			UE_LOG(LogTemp, Warning, TEXT("Target actor is invalid, ending lock-on"));
			EndLockon();
		}
		return;
	}

	FVector CurrentLocation = OwnerRef->GetActorLocation();
	FVector TargetLocation = CurrentTargetActor->GetActorLocation();

	
	double TargetDistance = FVector::Distance(CurrentLocation, TargetLocation);

	if (TargetDistance >= BreakDistance)
	{
		EndLockon();
		return;
	}

	TargetLocation.Z -= 125;

	FRotator NewRotation = UKismetMathLibrary::FindLookAtRotation(CurrentLocation, TargetLocation);

	if (Controller)
	{
		Controller->SetControlRotation(NewRotation);
	}

	// ...
}

