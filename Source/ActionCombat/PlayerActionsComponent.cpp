// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerActionsComponent.h"
#include "GameFramework/Character.h"
#include "MainPlayer.h"
#include "Kismet/KismetMathLibrary.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/CapsuleComponent.h"

// Sets default values for this component's properties
UPlayerActionsComponent::UPlayerActionsComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}

void UPlayerActionsComponent::Sprint()
{
	if (!IPlayerRef->HasEnoughStamina(SprintCost))
	{
		Walk();
		return;
	}

	if (MovementComp->Velocity.Equals(FVector::ZeroVector, 1))
	{
		return;
	}

	MovementComp->MaxWalkSpeed = SprintSpeed;
	

	//EVEN BROADCAST ?? Delegate Event가 호출됨 
	OnSprintDelegate.Broadcast(SprintCost);
}

void UPlayerActionsComponent::Walk()
{

		MovementComp->MaxWalkSpeed = WalkSpeed;

}

void UPlayerActionsComponent::Roll()
{
	if (bIsRollActive || !IPlayerRef->HasEnoughStamina(RollCost))
	{
		return;
	}
	
	bIsRollActive = true;

	OnRollDelegate.Broadcast(RollCost);

	FVector Direction = CharacterRef->GetCharacterMovement()->Velocity.Length() < 1 ? CharacterRef->GetActorForwardVector() : CharacterRef->GetLastMovementInputVector();

	FRotator NewRot = UKismetMathLibrary::MakeRotFromX(Direction);
	
	CharacterRef->SetActorRotation(NewRot);
	
	float Duration = CharacterRef->PlayAnimMontage(RollAnimMontage);

	FTimerHandle RollTimerHandle;

	CharacterRef->GetWorldTimerManager().SetTimer(RollTimerHandle, this, &UPlayerActionsComponent::FinishRollAnim, Duration, false);

}

void UPlayerActionsComponent::FinishRollAnim()
{
	bIsRollActive = false;
}

//
void UPlayerActionsComponent::SetCapsuleSizeDown()
{	
	ACharacter* OwnerCharacter = Cast<ACharacter>(GetOwner());

	if (OwnerCharacter)
	{
		UCapsuleComponent* CapsuleComp = OwnerCharacter->GetCapsuleComponent();

		if (CapsuleComp)
		{
			CapsuleComp->SetCapsuleHalfHeight(44.0f);
		}
	}

}

void UPlayerActionsComponent::SetCapsuleSizeNormal()
{
		ACharacter* OwnerCharacter = Cast<ACharacter>(GetOwner());

	if (OwnerCharacter)
	{
		UCapsuleComponent* CapsuleComp = OwnerCharacter->GetCapsuleComponent();

		if (CapsuleComp)
		{
			CapsuleComp->SetCapsuleHalfHeight(88.0f);
		}
	}

}
//


// Called when the game starts
void UPlayerActionsComponent::BeginPlay()
{
	Super::BeginPlay();

	CharacterRef = GetOwner<ACharacter>();

	MovementComp = CharacterRef->GetCharacterMovement();

	if(!CharacterRef->Implements<UMainPlayer>())
	{
		return;
	}
	
	IPlayerRef = Cast<IMainPlayer>(CharacterRef);
}


// Called every frame
void UPlayerActionsComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

