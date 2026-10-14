// Fill out your copyright notice in the Description page of Project Settings.


#include "MainCharacter.h"
#include "PlayerAnimInstance.h"
#include "StatsComponent.h"
#include "EStat.h"

#include "LockonComponent.h"
#include "CombatComponent.h"
#include "TraceComponent.h"
#include "BlockComponent.h"
#include "PlayerActionsComponent.h"

#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"

#include "Kismet/GameplayStatics.h"




// Sets default values
AMainCharacter::AMainCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	StatsComp = CreateDefaultSubobject<UStatsComponent>(TEXT("Stats Component"));
	LockonComp = CreateDefaultSubobject<ULockonComponent>(TEXT("Lockon Component"));
	CombatComp = CreateDefaultSubobject<UCombatComponent>(TEXT("Combat Component"));
	//TraceComp = CreateDefaultSubobject<UTraceComponent>(TEXT("Trace Component"));
	BlockComp = CreateDefaultSubobject<UBlockComponent>(TEXT("Block Component"));
	PlayerActionsComp = CreateDefaultSubobject<UPlayerActionsComponent>(TEXT("Player Actions Component"));



}

// Called when the game starts or when spawned
void AMainCharacter::BeginPlay()
{
	Super::BeginPlay();

	PlayerAnim = Cast<UPlayerAnimInstance>(GetMesh()->GetAnimInstance());

	SpringArmComp = FindComponentByClass<USpringArmComponent>();
	CameraComp = FindComponentByClass<UCameraComponent>();

	if (SpringArmComp)
	{	
		UE_LOG(LogTemp, Warning, TEXT("SpringArmComp is valid"));
		SpringArmComp->TargetArmLength = 300.0f;
	}

	if (CameraComp)
    {	
		UE_LOG(LogTemp, Warning, TEXT("CameraComp is valid"));
        CameraComp->SetFieldOfView(95.0f);
    }



}

// Called every frame
void AMainCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (SpringArmComp)
	{
		// 상태별 타겟 거리
		if (bIsAttacking)
		{
			CurrentTargetArmLength = AttackArmLength;
		}
		else if (bIsSprinting)
		{
			CurrentTargetArmLength = SprintArmLength;
		}
		else
		{
			CurrentTargetArmLength = NormalArmLength;
		}

		// 부드럽게 보간
		SpringArmComp->TargetArmLength = FMath::FInterpTo(
			SpringArmComp->TargetArmLength,
			CurrentTargetArmLength,
			DeltaTime,
			ZoomInterpSpeed
		);
	}

}

// Called to bind functionality to input
void AMainCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

float AMainCharacter::GetDamage()
{
	return StatsComp->Stats[EStat::Strength];
}

void AMainCharacter::GetInstantDeathDamage()
{
	FDamageEvent TargetAttackedEvent;

	PlayerAnim->bIsBlocking = false;

	this->TakeDamage(
		999,
		TargetAttackedEvent,
		nullptr,
		nullptr
	);
}

void AMainCharacter::IncreaseDamage()
{
	StatsComp->IncreaseStrength(5.f);
}

bool AMainCharacter::HasEnoughStamina(float Cost) 
{
	return StatsComp->Stats[EStat::Stamina] >= Cost;
}

void AMainCharacter::HandleDeath()
{
	PlayAnimMontage(DeathAnimMontage);

	DisableInput(GetController<APlayerController>());
}

void AMainCharacter::EndLockonWithActor(AActor* ActorRef)
{
	if (LockonComp->CurrentTargetActor != ActorRef)
	{
		return;
	}

	LockonComp->EndLockon();
}

bool AMainCharacter::CanTakeDamage(AActor* Opponent)
{	
	if (PlayerActionsComp->bIsRollActive)
	{
		return false;
	}

	if (PlayerAnim->bIsBlocking)
	{
		return BlockComp->Check(Opponent); // 마주하고 있는지
	}

	return true;
}

void AMainCharacter::PlayHurtAnim(TSubclassOf<UCameraShakeBase>CameraShakeTemplate)
{
	PlayAnimMontage(HurtAnimMontage);

	if (CameraShakeTemplate)
	{
		GetController<APlayerController>()
		->ClientStartCameraShake(CameraShakeTemplate);
	} 
}

void AMainCharacter::playCameraShake(TSubclassOf<UCameraShakeBase> CameraShakeTemplate)
{
	if (CameraShakeTemplate)
	{
		GetController<APlayerController>()
		->ClientStartCameraShake(CameraShakeTemplate);
	}
}

void AMainCharacter::SetCameraFocusOnTarget(AActor* TargetActor)
{
	if (!TargetActor || !SpringArmComp)
	{	
		UE_LOG(LogTemp, Warning, TEXT("TargetActor or SpringArmComp is null"));
		return;
	}
    
	//UE_LOG(LogTemp, Warning, TEXT("Setting camera focus on target actor: %s"), *TargetActor->GetName());

	//
	GetCharacterMovement()->DisableMovement();


		// 원래 회전 저장
	if (AController* MyController = GetController())
	{
		OriginalControlRotation = MyController->GetControlRotation();
	}
	if (CameraComp)
	{
		OriginalCameraRotation = CameraComp->GetComponentRotation();
	}



    FVector TargetLocation = TargetActor->GetActorLocation();
    FVector MyLocation = GetActorLocation();

    // 방향 벡터 계산
    FVector Direction = (TargetLocation - MyLocation).GetSafeNormal();
    
    // 회전 계산
    FRotator LookAtRotation = Direction.Rotation();
	if (CameraComp)
	{	
		UE_LOG(LogTemp, Warning, TEXT("CameraComp is valid 2"));
		CameraComp->SetWorldRotation(LookAtRotation);
	}
}

void AMainCharacter::RestoreOriginalView()
{
	if (AController* MyController = GetController())
	{
		MyController->SetControlRotation(OriginalControlRotation);
	}
	if (CameraComp)
	{
		CameraComp->SetWorldRotation(OriginalCameraRotation);
	}
}


void AMainCharacter::HideHeadBone()
{	
	GetMesh()->HideBoneByName(FName("head"), EPhysBodyOp::PBO_None);

	if (HeadOutParticleTemplate)
	{
		UGameplayStatics::SpawnEmitterAtLocation(
			GetWorld(),
			HeadOutParticleTemplate,
			GetMesh()->GetSocketLocation(FName("head"))
		);
	}
}

