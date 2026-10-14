// Fill out your copyright notice in the Description page of Project Settings.


#include "BossCharacter.h"
#include "StatsComponent.h"
#include "MainCharacter.h"
#include "Kismet/KismetMathLibrary.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "CombatComponent.h"
#include "TraceComponent.h"
#include "MainCharacter.h"
#include "BrainComponent.h"
#include "Components/CapsuleComponent.h"
#include "MainPlayer.h"

#include "MainGameInstance.h"
#include "Kismet/GameplayStatics.h"

#include "Components/SkeletalMeshComponent.h"

// Sets default values
ABossCharacter::ABossCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;


	
	CombatComp = CreateDefaultSubobject<UCombatComponent>(TEXT("Combat Component"));
	StatsComp = CreateDefaultSubobject<UStatsComponent>(TEXT("Stats Component"));
	//TraceComp = CreateDefaultSubobject<UTraceComponent>(TEXT("Trace Component"));



}

void ABossCharacter::ShowPlayerHeadBone()
{
    if (!HeadStaticMesh) return; // 에셋이 없으면 실행 X

    UStaticMeshComponent* NewMeshComp = NewObject<UStaticMeshComponent>(this);
    if (!NewMeshComp) return;

    NewMeshComp->RegisterComponent();
    NewMeshComp->SetStaticMesh(HeadStaticMesh);

    // 소켓에 부착
    NewMeshComp->AttachToComponent(GetMesh(),
        FAttachmentTransformRules::SnapToTargetIncludingScale,
        HandSocketName);

    // 상대 회전 180도 적용 (Z축 기준)
    NewMeshComp->SetRelativeRotation(FRotator(0.f, 180.f, 0.f));
}

void ABossCharacter::StartRandomTimer()
{
	float RandomDelay = FMath::RandRange(10.0f, 15.0f);

	GetWorldTimerManager().SetTimer(
		RandomTimerHandle,
		this,
		&ABossCharacter::OnRandomTick,
		RandomDelay,
		false
	);
}

void ABossCharacter::OnRandomTick()
{	
	UE_LOG(LogTemp, Warning, TEXT("Random Switch To Mega Attack"));

	SetToMegaAttack();

	StartRandomTimer();
}



// Called when the game starts or when spawned
void ABossCharacter::BeginPlay()
{	
	Super::BeginPlay();

	AAIController* AIController = GetController<AAIController>();
	if (AIController && AIController->GetBlackboardComponent())
	{
		BlackboardComp = AIController->GetBlackboardComponent();
		BlackboardComp->SetValueAsEnum(TEXT("CurrentState2"), InitialState);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("ABossCharacter: Failed to get valid AIController or BlackboardComponent!"));
	}
	
	GameInstance = Cast<UMainGameInstance>(GetGameInstance());

	StartRandomTimer();


}


// Called every frame
void ABossCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	

}

// Called to bind functionality to input
void ABossCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

///
void ABossCharacter::DetectPawn(APawn* DetectedPawn, APawn* PawnToDetect)
{	
	EEnemyState CurrentState = static_cast<EEnemyState>(BlackboardComp->GetValueAsEnum(TEXT("CurrentState2")));

	if (DetectedPawn == nullptr || PawnToDetect == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("ABossCharacter: DetectedPawn or PawnToDetect is null!"));
	
		return;
	}

	if (DetectedPawn != PawnToDetect || CurrentState != EEnemyState::Idle)
	{
		return;
	}

	UE_LOG(LogTemp, Warning, TEXT("Detected Pawn: %s"), *DetectedPawn->GetName());
	BlackboardComp->SetValueAsEnum(TEXT("CurrentState2"), EEnemyState::Range);
}

float ABossCharacter::GetDamage()
{
	return StatsComp->Stats[EStat::Strength];
}

void ABossCharacter::Attack()
{	
	UE_LOG(LogTemp, Warning, TEXT("BossCharacter Attack called"));
	CombatComp->RandomAttack();
}

void ABossCharacter::PerformMegaAttack()
{
    if (!MegaAttackAnim)
    {
        return;
    }

	if (!GameInstance)
	{
		return;
	}

    GameInstance->bIsMegaAttack = true;

    float Duration = PlayAnimMontage(MegaAttackAnim);

    UE_LOG(LogTemp, Warning, TEXT("BossCharacter PerformMegaAttack called"));

    // 몽타주 끝나면 bIsMegaAttack을 false로 되돌리는 타이머 설정
    FTimerHandle MegaAttackTimerHandle;
    GetWorld()->GetTimerManager().SetTimer(
        MegaAttackTimerHandle,
        this,
        &ABossCharacter::FinishMegaAttack,
        4.5f, // 몽타주 길이와 동일하게 설정
        false
    );
	
}






//
float ABossCharacter::GetAnimDuration()
{
	return CombatComp->AnimDuration;
}


void ABossCharacter::HandlePlayerDeath()
{
	GetController<AAIController>()->GetBlackboardComponent()->SetValueAsEnum(TEXT("CurrentState"), EEnemyState::GameOver);
}

void ABossCharacter::HandleDeath()
{	
	OnEnemyDied.Broadcast(); 

	float Duration = PlayAnimMontage(DeathAnim);

	AAIController* AIController = GetController<AAIController>();

	// Stop AI Logic
	AIController->GetBrainComponent()->StopLogic(TEXT("Defeated"));

	// Diable Collision
	FindComponentByClass<UCapsuleComponent>()
		->SetCollisionEnabled(ECollisionEnabled::NoCollision);

		
		FTimerHandle DestroyTimerHandle;

		GetWorld()->GetTimerManager().SetTimer(
			DestroyTimerHandle, 
			this, 
			&ABossCharacter::FinishDeathAnim, 
			Duration, 
			false);

			IMainPlayer* PlayerRef =
			GetWorld()->GetFirstPlayerController()
			->GetPawn<IMainPlayer>();

	if (!PlayerRef)
	{
		return;
	}

	PlayerRef->EndLockonWithActor(this);
}

void ABossCharacter::FinishDeathAnim()
{
	Destroy();
}

void ABossCharacter::FinishMegaAttack()
{
	if (GameInstance)
    {
        GameInstance->bIsMegaAttack = false;
        UE_LOG(LogTemp, Warning, TEXT("MegaAttack finished, bIsMegaAttack set to false"));
    }

	BlackboardComp->SetValueAsEnum(TEXT("CurrentState2"), EEnemyState::Melee);
}

void ABossCharacter::SetToMegaAttack()
{	
	EEnemyState CurrentState = static_cast<EEnemyState>(BlackboardComp->GetValueAsEnum(TEXT("CurrentState2")));

	if (CurrentState != EEnemyState::Range && CurrentState != EEnemyState::Melee)
	{
		UE_LOG(LogTemp, Warning, TEXT("ABossCharacter: Cannot set to MegaAttack from current state!"));
		return;
	}
	BlackboardComp->SetValueAsEnum(TEXT("CurrentState2"), EEnemyState::MegaAttack);
}






