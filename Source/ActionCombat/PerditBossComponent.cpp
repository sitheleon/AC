// Fill out your copyright notice in the Description page of Project Settings.


#include "PerditBossComponent.h"
#include "Engine/Engine.h"
#include "TimerManager.h"

#include "DrawDebugHelpers.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/Controller.h"
#include "GameFramework/Character.h"

#include "DrawDebugHelpers.h"
#include "TimerManager.h"


#include "NiagaraFunctionLibrary.h"


// Sets default values for this component's properties
UPerditBossComponent::UPerditBossComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you n't need them.
	PrimaryComponentTick.bCanEverTick = false;

	// ...
}

void UPerditBossComponent::StartPhase2Loop()
{
	if (bRunning)
	{
		return;
	}

	

	bRunning = true;


	GetWorld()->GetTimerManager().SetTimer(
		LoopHandle, this, &UPerditBossComponent::LoopTick, 0.25f, true, 0.0f);

	
		UE_LOG(LogTemp, Error,TEXT("[Phase 2] Loop Start"));
	
		ActivateAuraVFX();
}

void UPerditBossComponent::StopPhase2Loop()
{
	if (!bRunning)
	{
		return; // if not running
	}

	
	GetWorld()->GetTimerManager().ClearTimer(LoopHandle);

	UE_LOG(LogTemp, Error, TEXT("[Phase 2] Loop STOP"));



}


// Called when the game starts
void UPerditBossComponent::BeginPlay()
{
	Super::BeginPlay();

	    UE_LOG(LogTemp, Warning, TEXT("[PerditBossComp] NS_Aura at BeginPlay: %s"),
           *GetNameSafe(NS_Aura));

	// ...
	
}

void UPerditBossComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	StopPhase2Loop();

	Super::EndPlay(EndPlayReason);
}	


// Called every frame
void UPerditBossComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

void UPerditBossComponent::LoopTick()
{
	++LoopCount;

	FString Msg = FString::Printf(TEXT("[P2] Tick #%d"), LoopCount);

	//UE_LOG(LogTemp, Error, TEXT("[P2] Tick #%d"), LoopCount);




	//AURA
	if (!bGroggy)
	{
		DealAuraDamage(0.25f);
	}

	if (bGroggy)
	{
		return;
	}

	Stamina = FMath::Clamp(Stamina + StaminaRegenPerSec * 0.25f, 0.f, MaxStamina);

	UE_LOG(LogTemp, Warning, TEXT("[P2] ST %.0f/%.0f"), Stamina, MaxStamina);

		// 4) 패턴 한 개 실행 시도
	const float Now = GetWorld()->GetTimeSeconds();
	EPerdit2Pattern P = SelectNextPattern();
	if (! IsOnCoolDown(P, Now))
	{
		RunPattern(P);
		ArmCooldown(P, Now);
	}

}

void UPerditBossComponent::DealAuraDamage(float DeltaSeconds)
{
	APawn* Player = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);

	if (!Player)
	{
		return;
	}

	const FVector Origin = GetOwner()->GetActorLocation();
	const float Dist = FVector::Dist(Player->GetActorLocation(), Origin);

	if (bDebugAura)
    {
        // 반투명 구로 범위 가시화 (0.25초 지속 → 루프 주기와 맞춰 항상 보임)
        DrawDebugSphere(GetWorld(), Origin, AuraRadius, 24, FColor::Purple, false, 0.25f, 0, 2.f);
    }



	//if in range
	if (Dist <= AuraRadius)
	{
		const float Dmg = AuraDPS * DeltaSeconds;

		AController* InstigatorCtrl = nullptr;

		if (ACharacter* BossChar = Cast<ACharacter>(GetOwner()))
		{
			InstigatorCtrl = BossChar->GetController();
		}

		UGameplayStatics::ApplyDamage(
			Player, Dmg, InstigatorCtrl, GetOwner(), AuraDamageType
		);

	}
}

float UPerditBossComponent::GetStamina() const
{
	return Stamina;
}

bool UPerditBossComponent::IsGroggy() const
{
	return bGroggy;
}

void UPerditBossComponent::ConsumeStamina(float Amount)
{
	if (bGroggy)
	{
		return;
	}

	Stamina = FMath::Clamp(Stamina - Amount, 0.f, MaxStamina);

	UE_LOG(LogTemp, Warning, TEXT("[P2] ST %.0f/%.0f"), Stamina, MaxStamina);

	if (Stamina <= 0.f)
	{
		EnterGroggy();
	}
}

void UPerditBossComponent::EnterGroggy()
{
	if (bGroggy)
	{
		return;
	}

	bGroggy = true;
	UE_LOG(LogTemp, Warning, TEXT("ENTERED GROGGY"));

	DeactivateAuraVFX();

	GetWorld()->GetTimerManager().SetTimer(
		GroggyHandle, this, &UPerditBossComponent::ExitGroggy, GroggyDuration, false
	);

}

void UPerditBossComponent::ExitGroggy()
{
	bGroggy = false;

	Stamina = MaxStamina * 0.5f;

	ActivateAuraVFX();
}




EPerdit2Pattern UPerditBossComponent::SelectNextPattern() const
{
	// 가중치: 실 (40%), W (35%), 봉인 (25%)

	const int R = FMath::RandRange(0, 99);

	if (R < 40)
	{
		return EPerdit2Pattern::RangedTreads;
	}

	if (R < 75)
	{
		return EPerdit2Pattern::XerathW;
	}

	return EPerdit2Pattern::SealSkills;
}

bool UPerditBossComponent::IsOnCoolDown(EPerdit2Pattern P, float New) const
{
	switch (P)
	{
		case EPerdit2Pattern::RangedTreads: return New < NextAvail_Ranged;

		case EPerdit2Pattern::XerathW: return New < NextAvail_Xerath;

		case EPerdit2Pattern::SealSkills: return New < NextAvail_Seal;

		default: return false;
	}
}

void UPerditBossComponent::ArmCooldown(EPerdit2Pattern P, float New)
{
	const float Gap = FMath::FRandRange(PatternGapMin, PatternGapMAx);

	switch (P)
	{
	case EPerdit2Pattern::RangedTreads: NextAvail_Ranged = New + RangedThreadsCD + Gap; break;
    case EPerdit2Pattern::XerathW:       NextAvail_Xerath = New + XerathWCD       + Gap; break;
    case EPerdit2Pattern::SealSkills:    NextAvail_Seal   = New + SealSkillCD; 

	}
}

void UPerditBossComponent::RunPattern(EPerdit2Pattern P)
{

#if WITH_EDITOR
    const TCHAR* Name =
        (P == EPerdit2Pattern::RangedTreads) ? TEXT("RangedThreads") :
        (P == EPerdit2Pattern::XerathW)       ? TEXT("XerathW") :
                                                TEXT("SealSkills");
    if (GEngine) GEngine->AddOnScreenDebugMessage(4, 0.8f, FColor::White,
        FString::Printf(TEXT("[P2] PATTERN: %s"), Name));
    UE_LOG(LogTemp, Warning, TEXT("[P2] PATTERN: %s"), Name);
#endif

	switch (P)
    {
    case EPerdit2Pattern::RangedTreads:  DoRangedThreads(); break;
    case EPerdit2Pattern::XerathW:       Do_XerathW(); break;
    case EPerdit2Pattern::SealSkills:    ConsumeStamina(10.f); break;
    }

}

void UPerditBossComponent::DoRangedThreads()
{
	if (!ThreadProjectileClass)
	{
		return;
	}

	const AActor* Owner = GetOwner();

	if (!Owner)
	{
		return;
	}


	if (ACharacter* OwnerChar = Cast<ACharacter>(GetOwner()))
	{	
		UE_LOG(LogTemp, Error, TEXT("Success"));
	
	}

/*
	if (ACharacter* C = Cast<ACharacter>(const_cast<AActor*>(Owner)))
	{	
		UE_LOG(LogTemp, Error, TEXT("FAILED TO CAST"));
		C->LaunchCharacter(FVector(0,0,2450.f), false, false);
	}
*/

	const FVector Muzzle = Owner->GetActorLocation() + FVector(0,0,120.f);
	APawn* Player = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
	if (!Player)
	{
		return;
	}

	const FVector AimDir = (Player->GetActorLocation() - Muzzle).GetSafeNormal();
	const FRotator Basis = AimDir.Rotation();

	const int32 Count = 8;
	const float SpreadYawDeg = 24.f;

	for (int32 i = 0; i < Count; ++i)
	{
		const float T = (Count==1)? 0.f : (float(i)/(Count-1)-0.5);
		const FRotator ShotRot = Basis + FRotator(0.f, T * SpreadYawDeg, 0.f);

		FActorSpawnParameters SP;
		SP.Owner = const_cast<AActor*>(Owner);
		SP.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

		if (!ThreadSound)
		{	
			UE_LOG(LogTemp, Warning, TEXT("No Thread Sound"));
			return;
		}

		//sound
		UGameplayStatics::PlaySoundAtLocation(
			this,
			ThreadSound,
			Muzzle,
			1.f,
			FMath::FRandRange(0.97f, 1.03f)
		);

		GetWorld()->SpawnActor<AActor>(ThreadProjectileClass, Muzzle, ShotRot, SP);

	}


	ConsumeStamina(30.f);
}

void UPerditBossComponent::Do_XerathW()
{
    UWorld* W = GetWorld();
    APawn* Player = UGameplayStatics::GetPlayerPawn(W, 0);
    if (!W || !Player || !NS_XerathMarker) return;

    const FVector TargetLoc = Player->GetActorLocation();
    const float   Radius    = 250.f;
    const float   Delay     = 1.0f;

    // 1) 마커 스폰 (AutoDestroy=false 로 두고 직접 파괴)
    UNiagaraComponent* Marker = UNiagaraFunctionLibrary::SpawnSystemAtLocation(
        W, NS_XerathMarker, TargetLoc, FRotator::ZeroRotator, FVector::OneVector,
        /*bAutoDestroy*/ false, /*bAutoActivate*/ true);

    if (Marker)
    {
        // 네 에셋이 언더스코어면 이름도 동일하게!
        Marker->SetVariableFloat(TEXT("User_Radius"),   Radius);
        Marker->SetVariableFloat(TEXT("User_Duration"), Delay);

        // 마커는 Delay 직후 제거 (조금의 여유를 주고 싶으면 +0.05f)
        SafeDestroyNiagara(W, Marker, Delay);
    }

    // 2) Delay 후 폭발 + 데미지
    FTimerHandle H;
    W->GetTimerManager().SetTimer(H, [this, Radius, TargetLoc]()
    {
        UWorld* World = GetWorld();
        if (!World) return;

        // 2-1) 폭발 스폰
        UNiagaraComponent* Explosion = nullptr;
        if (NS_XerathExplosion)
        {
            Explosion = UNiagaraFunctionLibrary::SpawnSystemAtLocation(
                World, NS_XerathExplosion, TargetLoc, FRotator::ZeroRotator, FVector::OneVector,
                /*bAutoDestroy*/ false, /*bAutoActivate*/ true);

            if (Explosion)
            {
                // (필요하면) 변수 세팅
                // Explosion->SetVariableFloat(TEXT("User_Radius"), Radius);

                // 폭발 예상 수명(예: 0.5s) 뒤 제거
                SafeDestroyNiagara(World, Explosion, 1.0f);
            }
        }

        // 2-2) 범위 데미지
        if (APawn* P = UGameplayStatics::GetPlayerPawn(World, 0))
        {
            if (FVector::Dist2D(P->GetActorLocation(), TargetLoc) <= Radius)
            {
                AController* Inst = nullptr;
                if (ACharacter* BossChar = Cast<ACharacter>(GetOwner()))
                    Inst = BossChar->GetController();

                UGameplayStatics::ApplyDamage(P, 25.f, Inst, GetOwner(), AuraDamageType);
            }
        }

    }, Delay, false);

    ConsumeStamina(30.f);
}


void UPerditBossComponent::ActivateAuraVFX()
{	

    if (!NS_Aura) 
	{
			
			UE_LOG(LogTemp, Error, TEXT("[AuraVFX] ERROR"));
			return;
	}
	
	

    AActor* Owner = GetOwner(); 
	
	if (!Owner) return;

    AuraComp = UNiagaraFunctionLibrary::SpawnSystemAttached(
        NS_Aura, Owner->GetRootComponent(), NAME_None,
        FVector::ZeroVector, FRotator::ZeroRotator,
        EAttachLocation::SnapToTarget, true);

    if (AuraComp)
    {
        AuraComp->SetVariableFloat(TEXT("User.Radius"), AuraRadius);
        UE_LOG(LogTemp, Error, TEXT("[AuraVFX] Spawned, Radius=%.1f"), AuraRadius);
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("[AuraVFX] Spawn FAILED"));
    }

}

void UPerditBossComponent::UpdateAuraRadius(float NewRadius)
{
	AuraRadius = NewRadius;
	if (AuraComp)
	{
		AuraComp->SetVariableFloat(TEXT("User.Radius"), AuraRadius);
	}
}

void UPerditBossComponent::DeactivateAuraVFX()
{
	if (AuraComp)
	{
		AuraComp->Deactivate();
		AuraComp->DestroyComponent();
		AuraComp = nullptr;
	}
}

void UPerditBossComponent::SafeDestroyNiagara(UWorld* W, UNiagaraComponent* Comp, float DelaySec)
{
	if (!W || !Comp) return;

    TWeakObjectPtr<UNiagaraComponent> Weak = Comp;
    FTimerHandle Th;
    W->GetTimerManager().SetTimer(Th, [Weak]()
    {
        if (!Weak.IsValid()) return;
        Weak->Deactivate();
        Weak->DestroyComponent();   // 완전 제거
    }, DelaySec, false);
}
