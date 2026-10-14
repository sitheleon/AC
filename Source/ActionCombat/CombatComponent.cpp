// Fill out your copyright notice in the Description page of Project Settings.


#include "CombatComponent.h"
#include "GameFramework/Character.h"
#include "Kismet/KismetMathLibrary.h"
#include "MainPlayer.h"

// Sets default values for this component's properties
UCombatComponent::UCombatComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}

void UCombatComponent::ComboAttack()
{	
								//"U" in Interface is for checking if it is validate or not
	if (CharacterRef->Implements<UMainPlayer>())
	{
		IMainPlayer* IPLayerRef = Cast<IMainPlayer>(CharacterRef);

		if (IPLayerRef && !IPLayerRef->HasEnoughStamina(StaminaCost))
		{
			return;
		}
	}

	if (!bCanAttack)
	{	
		UE_LOG(LogTemp, Warning ,TEXT("CANT ATTACK"));
		// can't attack
		
		return;
	}

	bCanAttack = false;
	
	CharacterRef->PlayAnimMontage(AttackAnimations[ComboCounter]);

	ComboCounter++;
					// number of items in TArray (AttackAnimations) == .Num()
	int32 MaxCombo = AttackAnimations.Num();

	ComboCounter = UKismetMathLibrary::Wrap(ComboCounter, -1, (MaxCombo-1));


	//Broadcast the event
	OnAttackPerformedDelegate.Broadcast(StaminaCost);

}

void UCombatComponent::PerformSpecialAttack()
{
	if (CharacterRef == nullptr)
	{
		return;
	}

	if (SpecialAttackAnimation == nullptr)
	{
		return;
	}

	CharacterRef->PlayAnimMontage(SpecialAttackAnimation);
}


// Called when the game starts
void UCombatComponent::BeginPlay()
{
	Super::BeginPlay();

	CharacterRef = GetOwner<ACharacter>();
	// ...
	
}


// Called every frame
void UCombatComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

void UCombatComponent::HandleResetAttack()
{
	bCanAttack = true;
}

void UCombatComponent::RandomAttack()
{
	
	    // CharacterRef 유효성 검사
    if (CharacterRef == nullptr)
    {
        UE_LOG(LogTemp, Error, TEXT("CharacterRef is null in RandomAttack!"));
        return;
    }

    // AttackAnimations 배열이 비어 있는지 확인
    if (AttackAnimations.Num() == 0)
    {
        UE_LOG(LogTemp, Error, TEXT("AttackAnimations is empty in RandomAttack!"));
        return;
    }

	int RandomIndex = FMath::RandRange(0, AttackAnimations.Num() - 1);

	//
	AnimDuration = CharacterRef->PlayAnimMontage(AttackAnimations[RandomIndex]);


}

