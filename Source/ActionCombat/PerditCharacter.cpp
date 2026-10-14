// Fill out your copyright notice in the Description page of Project Settings.


#include "PerditCharacter.h"
#include "StatsComponent.h"
#include "CableComponent.h"

#include "MainGameInstance.h"

#include "PerditBossComponent.h"

#include "AIController.h"

#include "BehaviorTree/BehaviorTreeComponent.h"
#include "BehaviorTree/BlackboardComponent.h"

#include "Kismet/GameplayStatics.h"

#include "Kismet/KismetMathLibrary.h"

#include "GameFramework/CharacterMovementComponent.h"

// Sets default values
APerditCharacter::APerditCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	StatsComp = CreateDefaultSubobject<UStatsComponent>(TEXT("Stats Component"));

	Thread = CreateDefaultSubobject<UCableComponent>(TEXT("Cable Component"));
	Thread->SetupAttachment(RootComponent); // 본체에 붙임
	Thread->SetVisibility(false);           // 처음엔 안 보이게 설정 (원하면 true)
	Thread->bEnableCollision = false;
	Thread->CableWidth = 5.f;
	Thread->NumSegments = 12;


}

// Called when the game starts or when spawned
void APerditCharacter::BeginPlay()
{
	Super::BeginPlay();

		GameInstance = Cast<UMainGameInstance>(GetGameInstance());

	TArray<UStaticMeshComponent*> MeshComponents;
	GetComponents<UStaticMeshComponent>(MeshComponents);

	for (UStaticMeshComponent* Comp : MeshComponents)
	{
		if (Comp->GetName().Contains(TEXT("Perdit_Maze")))
		{
			OrbMesh = Comp;
			OrbStartLocation = OrbMesh->GetComponentLocation();
			OrbRelativeOffset = OrbMesh->GetComponentLocation() - GetActorLocation(); // Calculate the relative location of the OrbMesh
			break;
		}
	}

	if (!OrbMesh)
	{
		UE_LOG(LogTemp, Warning, TEXT("OrbMesh not found!"));
		return;
	}


	TArray<UStaticMeshComponent*> EyeComponents;
	GetComponents<UStaticMeshComponent>(EyeComponents);

	for (UStaticMeshComponent* Comp : EyeComponents)
	{
		if (Comp->GetName().Contains(TEXT("Perdit_Eye")))
		{
			EyeMeshes.Add(Comp);
		}
	}

	if (EyeMeshes.Num() == 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("No EyeMeshes found!"));
		return;
	}
		
	StartLocation = GetActorLocation(); // Store the initial location of the character

	// 초기화
	CurrentSpawnedEnemy = nullptr;


}


// Called every frame
void APerditCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	

	//sMoveOrb(DeltaTime);

	//TemporarilyHideRandomEye(1.0f);

	HoverMotion(DeltaTime); // Call the hover motion function
	/*
	if (GameInstance)
	{
		UE_LOG(LogTemp, Error, TEXT("IsBossInitiated: %s"), GameInstance->bIsBossInitiated ? TEXT("true") : TEXT("false"));
	}
	*/
}

// Called to bind functionality to input
void APerditCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

float APerditCharacter::GetDamage()
{
	return StatsComp->Stats[EStat::Strength];
}

void APerditCharacter::DetectPawn(APawn* DetectedPawn, APawn* PawnToDetect)
{
	if (DetectedPawn == nullptr || PawnToDetect == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("ABossCharacter: DetectedPawn or PawnToDetect is null!"));
	
		return;
	}

	if (DetectedPawn != PawnToDetect)
	{
		return;
	}

}

void APerditCharacter::SpawnEnemy()
{
	if (GameInstance)
	{	
		if (GameInstance->bIsBossInitiated)
		{
			// 이미 소환된 적이 있으면 스폰하지 않음
			if (CurrentSpawnedEnemy && IsValid(CurrentSpawnedEnemy))
			{
				return;
			}

			if (!EnemyToSpawn)
			{
				UE_LOG(LogTemp, Warning, TEXT("EnemyToSpawn is not set!"));
				return;
			}

			// 라인트레이스로 지면 확인
			FVector TraceStart = GetActorLocation() + GetActorForwardVector() * 300.0f;
			FVector TraceEnd = TraceStart - FVector(0.0f, 0.0f, 1000.0f);

			FHitResult Hit;
			FCollisionQueryParams Params;
			Params.AddIgnoredActor(this);

			bool bHit = GetWorld()->LineTraceSingleByChannel(
				Hit,
				TraceStart,
				TraceEnd,
				ECollisionChannel::ECC_GameTraceChannel2,
				Params
			);

			if (!bHit)
			{	
				UE_LOG(LogTemp, Warning, TEXT("Line trace did not hit anything!"));
				return;
			}

			const float HalfHeight = 80.0f;
			FVector SpawnLocation = Hit.ImpactPoint + FVector(0, 0, HalfHeight);

			FVector RandomOffset = FVector(
				FMath::FRandRange(-100.f, 200.f),
				FMath::FRandRange(-100.f, 200.f),
				0.f
			);
			SpawnLocation += RandomOffset;

			FRotator SpawnRotation = FRotator::ZeroRotator;

			ACharacter* PlayerChar = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0);

			if (!PlayerChar)
			{
				return;
			}

			FVector CurrentLocation = GetActorLocation();
			FVector PlayerLocation = PlayerChar->GetActorLocation();

			SpawnRotation = UKismetMathLibrary::FindLookAtRotation(SpawnLocation, PlayerLocation);

			FActorSpawnParameters SpawnParams;
			SpawnParams.SpawnCollisionHandlingOverride =
				ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

			// ✅ Spawn Enemy
			CurrentSpawnedEnemy = GetWorld()->SpawnActor<ACharacter>(
				EnemyToSpawn, SpawnLocation, SpawnRotation, SpawnParams);

			if (CurrentSpawnedEnemy)
			{
				// 적이 죽었을 때 콜백 등록
				CurrentSpawnedEnemy->OnDestroyed.AddDynamic(this, &APerditCharacter::OnSpawnedEnemyDestroyed);
				
				if (Thread)
				{
					Thread->SetVisibility(true);
					Thread->SetAttachEndTo(CurrentSpawnedEnemy, FName("RootComponent"));
					Thread->CableLength = FVector::Dist(GetActorLocation(), CurrentSpawnedEnemy->GetActorLocation()) + 50.f;

					if (CableMaterial)
					{
						Thread->SetMaterial(0, CableMaterial);
					}
				}
			}
		}
	}
}



void APerditCharacter::MoveOrb(float DeltaTime)
{
	if (!OrbMesh)
	{
		return;
	}	

	ElapsedTime += DeltaTime;

	float Amplitude = 10.0f;
	float Speed = 2.0f;

	float Offset = FMath::Sin(ElapsedTime * Speed) * Amplitude;

	FVector OrbBaseLocation = GetActorLocation() + OrbRelativeOffset; // Use the relative offset to calculate the base location
	OrbBaseLocation.Z += Offset; // Move up and down along the Z-axis

	OrbMesh->SetWorldLocation(OrbBaseLocation); // Set the new location of the OrbMesh


}




void APerditCharacter::TemporarilyHideRandomEye(float HideDuration)
{
	if (EyeMeshes.Num() == 0)
	{
		return;
	}

	int32 RandomIndex = FMath::RandRange(0, EyeMeshes.Num() - 1);
	UStaticMeshComponent* TargetEye = EyeMeshes[RandomIndex];

	if(!TargetEye)
	{
		return;
	}
	
	FVector OriginalLocation = TargetEye->GetComponentLocation();

	TargetEye->SetWorldScale3D(FVector::ZeroVector); // Hide the eye by scaling it down to zero

	FTimerHandle TimerHandle;
	FTimerDelegate TimerDel;

	TimerDel.BindLambda([this, TargetEye, OriginalLocation]()
	{
		if (TargetEye)
		{
			TargetEye->SetWorldScale3D(FVector(1.0f)); // Restore the eye scale
			TargetEye->SetWorldLocation(OriginalLocation); // Restore the original location
		}
	});
	GetWorld()->GetTimerManager().SetTimer(TimerHandle, TimerDel, HideDuration, false); // Set a timer to restore the eye after HideDuration		

}

void APerditCharacter::HoverMotion(float DeltaTime)
{	
	if (bVibrating)
	{
		return;
	}

	HoverElapsedTime += DeltaTime;

	float HoverAmplitude = 10.0f; // Amplitude of the hover motion
	float HoverSpeed = 1.5f;

	float Offset = FMath::Sin(HoverElapsedTime * HoverSpeed) * HoverAmplitude;

	FVector NewLocation = StartLocation;
	NewLocation.Z += Offset; // Move up and down along the Z-axis

	SetActorLocation(NewLocation); // Set the new location of the character
}



void APerditCharacter::OnSpawnedEnemyDestroyed(AActor* DestroyedActor)
{
	UE_LOG(LogTemp, Warning, TEXT("Spawned enemy destroyed, hiding thread and preparing for next spawn"));
	
	// 현재 소환된 적 참조 제거
	CurrentSpawnedEnemy = nullptr;
	
	// 케이블 숨기기
	if (Thread)
	{
		Thread->SetVisibility(false);
	}
	
	// 잠시 후 새로운 적 스폰 (약간의 딜레이를 두어 자연스럽게)
	FTimerHandle DelaySpawnTimer;
	GetWorld()->GetTimerManager().SetTimer(
		DelaySpawnTimer, 
		this, 
		&APerditCharacter::SpawnEnemy, 
		3.0f,  // 3초 후 재스폰
		false
	);
}

void APerditCharacter::EndPhase2(UBehaviorTreeComponent* BTC)
{	

	if (!BTC)
	{
		return;
	}

	if (UBlackboardComponent* BB = BTC->GetBlackboardComponent())
	{
		BB->SetValueAsBool(TEXT("Phase2Active"), false);
	}
}

void APerditCharacter::VibrateTick()
{
	if (!GetWorld())
	{
		return;
	}

	VibrateElapsed += 0.016f;
	const float T = FMath::Clamp(VibrateElapsed / VibrateDuration, 0.f, 1.f);

	const float Damper = FMath::Exp(-6.f * T);

	const float Osc = FMath:: Sin((8.f * PI)*T);
	const float Amp = VibrateAmplitude * Damper;

	const FVector Offset =
		(VibrateDir * Osc * Amp) +
		FVector(0,0,0.25f*Osc*Amp);

	SetActorLocation(VibrateOrigin + Offset, true);

	if (T>=1.f)
	{
		SetActorLocation(VibrateOrigin, true);
		GetWorld()->GetTimerManager().ClearTimer(VibrateTimer);
		bVibrating = false;
	}
}

// 1) BP에서 직접 호출하는 쪽 (UFUNCTION)
void APerditCharacter::Vibrate()
{
    UE_LOG(LogTemp, Error, TEXT("[VIBRATE 0] wrapper called (self=%s)"), *GetName());

    const FVector Fwd = GetActorForwardVector();
    Vibrate(Fwd * -1.f, 1.f); // 반드시 여기로 들어가야 함
}

// 2) 실제 구현
void APerditCharacter::Vibrate(const FVector& HitFromDirection, float StrengthScale)
{
    UE_LOG(LogTemp, Error, TEXT("[VIBRATE 1] impl begin (self=%s)"), *GetName());

    if (!GetWorld())
    {
        UE_LOG(LogTemp, Error, TEXT("[VIBRATE 1] GetWorld() == nullptr"));
        return;
    }

    bVibrating = true;
    GetWorld()->GetTimerManager().ClearTimer(VibrateTimer);

    VibrateOrigin = GetActorLocation();
    VibrateElapsed = 0.0f;
    VibrateDir = HitFromDirection.GetSafeNormal2D();

    // 멀티플레이 컨텍스트 확인
    const bool bAuth = HasAuthority();
    UE_LOG(LogTemp, Error, TEXT("[VIBRATE 1] dir=%s, world=%s, HasAuth=%d"),
        *VibrateDir.ToString(), *GetWorld()->GetName(), bAuth ? 1 : 0);

    if (UCharacterMovementComponent* Move = GetCharacterMovement())
    {
        const FVector KnockXY = VibrateDir * (KnockbackStrength * StrengthScale);
        const FVector KnockZ  = FVector(0,0,120.f * StrengthScale);
        LaunchCharacter(KnockXY + KnockZ, false, true);
    }

    GetWorld()->GetTimerManager().SetTimer(
        VibrateTimer, this, &APerditCharacter::VibrateTick, 0.016f, true
    );

    // 화면 메시지: 서버/클라 분리
    if (GEngine)
    {
        // 서버에서 실행 중이면 서버 뷰포트에만 뜹니다 (전용서버는 화면 없음)
        GEngine->AddOnScreenDebugMessage(-1, 0.8f, FColor::Yellow, TEXT("VIBRATE!"));
    }
}

float APerditCharacter::GetHealth()
{
	return StatsComp->Stats[EStat::Health];
}
