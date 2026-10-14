// Fill out your copyright notice in the Description page of Project Settings.


#include "BlockComponent.h"
#include "GameFramework/Character.h"
#include "MainPlayer.h"


// Sets default values for this component's properties
UBlockComponent::UBlockComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.       
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UBlockComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}


// Called every frame
void UBlockComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

bool UBlockComponent::Check(AActor* Opponent)
{
	ACharacter* CharacterRef = GetOwner<ACharacter>();

	if (!CharacterRef->Implements<UMainPlayer>())
	{
		return true;
	}

	IMainPlayer* PlayerRef = Cast<IMainPlayer>(CharacterRef);

	FVector OpponentFoward = Opponent->GetActorForwardVector();
	FVector PlayerFoward = CharacterRef->GetActorForwardVector();

	double Result = FVector::DotProduct(OpponentFoward, PlayerFoward);


	if (Result > 0 || !PlayerRef->HasEnoughStamina(StaminaCost))
	{
		return true; // Take damage
	}


	

	CharacterRef->PlayAnimMontage(BlockAnimMontage);

	OnBlockDelegate.Broadcast(StaminaCost); //for reducing stamina

	return false;
}