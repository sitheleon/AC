#include "TraceComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/KismetMathLibrary.h"
#include "Fighter.h"
#include "Kismet/GameplayStatics.h"
#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "GameFramework/Character.h"

// Sets default values for this component's properties
UTraceComponent::UTraceComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

// Called when the game starts
void UTraceComponent::BeginPlay()
{
	Super::BeginPlay();

	UE_LOG(LogTemp, Warning, TEXT("TraceComponent BeginPlay called."));

	TArray<USkeletalMeshComponent*> SkeletalComponents;
	GetOwner()->GetComponents<USkeletalMeshComponent>(SkeletalComponents);

	// 1순위: Katana라는 이름의 무기 메시 찾기
	for (USkeletalMeshComponent* Comp : SkeletalComponents)
	{
		if (Comp && Comp->GetName() == SkeletalMeshComponentName.ToString())
		{
			SkeletalComp = Comp;
			UE_LOG(LogTemp, Warning, TEXT("Found specific skeletal mesh component: %s"), *SkeletalMeshComponentName.ToString());
			break;
		}
	}

	// 2순위: 못 찾았으면 첫 번째 SkeletalMeshComponent를 사용 (보스용 fallback)
	if (!SkeletalComp && SkeletalComponents.Num() > 0)
	{
		SkeletalComp = SkeletalComponents[0];
		UE_LOG(LogTemp, Warning, TEXT("Fallback to default skeletal mesh component: %s"), *SkeletalComp->GetName());
	}

	// 여전히 없으면 경고
	if (!SkeletalComp)
	{
		UE_LOG(LogTemp, Error, TEXT("No valid SkeletalMeshComponent found on %s"), *GetOwner()->GetName());
	}

	UE_LOG(LogTemp, Error, TEXT("SkeletalComp: %s"), *SkeletalComp->GetName());
}
// Called every frame
void UTraceComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (!bIsAttacking)
	{	
		bHasBroadcastedThisAttack = false; // 공격이 끝나면 플래그 리셋
		bLineTraceHit = false; // 공격 종료 시 히트 상태 초기화
		LastHitActor = nullptr;
		return;
	}

	// 공격 시작 시에만 한 번 브로드캐스트
	if (!bHasBroadcastedThisAttack)
	{
		OnLinetracePerformedDelegate.Broadcast();
		bHasBroadcastedThisAttack = true;
	}

	TArray<FHitResult> AllResults;
	bool bAnyHit = false; // 이번 프레임에 히트가 있었는지 추적

	for (const FTraceSockets& Socket : Sockets)
	{	
		UE_LOG(LogTemp, Warning, TEXT("Processing socket: %s to %s"), *Socket.Start.ToString(), *Socket.End.ToString());

		if (!SkeletalComp) 
		{	
			UE_LOG(LogTemp, Error, TEXT("SkeletalComp is null, cannot perform trace."));
			continue;
		}

		FVector StartSocketLocation = SkeletalComp->GetSocketLocation(Socket.Start);
		FVector EndSocketLocation = SkeletalComp->GetSocketLocation(Socket.End);
		FQuat ShapeRotation = SkeletalComp->GetSocketQuaternion(Socket.Rotation);

		double WeaponDistance = FVector::Distance(StartSocketLocation, EndSocketLocation);
		FVector BoxHalfExtend{ BoxCollisionLength, BoxCollisionLength, WeaponDistance };
		BoxHalfExtend /= 2;

		FCollisionShape Box = FCollisionShape::MakeBox(BoxHalfExtend);

		FCollisionQueryParams IgnoreParams{
			FName{ TEXT("Ignore Params") },
			false,
			GetOwner()
		};

		TArray<FHitResult> OutResults;

		bool bHasFoundTargets = GetWorld()->SweepMultiByChannel(
			OutResults,
			StartSocketLocation,
			EndSocketLocation,
			ShapeRotation,
			ECollisionChannel::ECC_GameTraceChannel1,
			Box,
			IgnoreParams
		);

		// 히트 발생 시 플래그 업데이트
		if (bHasFoundTargets && OutResults.Num() > 0)
		{
			bAnyHit = true;
		}

		for (FHitResult Hit : OutResults)
		{
			AllResults.Add(Hit);
			if (Hit.GetActor())
			{
				UE_LOG(LogTemp, Warning, TEXT(">> Hit Actor: %s"), *Hit.GetActor()->GetName());
			}
		}

		// 디버그 박스
		if (bDebugMode)
		{	
			UE_LOG(LogTemp, Warning, TEXT("DEBUG MODE"));

			FVector CenterPoint = UKismetMathLibrary::VLerp(StartSocketLocation, EndSocketLocation, 0.5f);

			UE_LOG(LogTemp, Warning, TEXT("Drawing Debug Box at: %s"), *CenterPoint.ToString());

			UKismetSystemLibrary::DrawDebugBox(
				GetWorld(),
				CenterPoint,
				Box.GetExtent(),
				bHasFoundTargets ? FLinearColor::Green : FLinearColor::Red,
				ShapeRotation.Rotator(),
				5.0f,    // Duration
				5.0f     // Line Thickness
			);
		}
	}

	// 라인트레이스 히트 상태 업데이트
	if (AllResults.Num() == 0)
	{
		bLineTraceHit = false;
		LastHitActor = nullptr;
		//UE_LOG(LogTemp, Warning, TEXT("No actors hit this frame."));
		return;
	}

	// 히트가 있었으므로 상태 업데이트
	bLineTraceHit = true;

	float CharacterDamage = 0.0f;
	IFighter* FighterRef = Cast<IFighter>(GetOwner());

	if (FighterRef)
	{
		CharacterDamage = FighterRef->GetDamage();
	}

	FDamageEvent TargetAttackedEvent;

	for (const FHitResult& Hit : AllResults)
	{
		AActor* TargetActor = Hit.GetActor();

		if (!TargetActor)
		{
			continue;
		}

		if (TargetsToIgnore.Contains(TargetActor))
		{
			//UE_LOG(LogTemp, Warning, TEXT("Ignoring already hit actor: %s"), *TargetActor->GetName());
			continue;
		}

		// 마지막으로 히트한 액터 업데이트
		LastHitActor = TargetActor;

		//UE_LOG(LogTemp, Warning, TEXT("Applying damage to: %s"), *TargetActor->GetName());

		TargetActor->TakeDamage(CharacterDamage, TargetAttackedEvent, GetOwner()->GetInstigatorController(), GetOwner());
		TargetsToIgnore.AddUnique(TargetActor);

		UGameplayStatics::SpawnEmitterAtLocation(
			GetWorld(),
			HitParticleTemplate,
			Hit.ImpactPoint
		);
	}


}

void UTraceComponent::HandleResetAttack()
{
	TargetsToIgnore.Empty();
}

void UTraceComponent::PlaySwordParticle()
{
	if (!SkeletalComp || !SwordParticleTemplate)
	{	
		UE_LOG(LogTemp, Error, TEXT("SkeletalComp or SwordParticleTemplate is null"));
		return;
	}

	for (const FTraceSockets& Socket : Sockets)
	{	
		const FVector StartLoc = SkeletalComp->GetSocketLocation(Socket.Start);
		const FVector EndLoc   = SkeletalComp->GetSocketLocation(Socket.End);

		const FVector MidPoint = (StartLoc + EndLoc) / 2.0f;
		const FRotator Rotation = (EndLoc - StartLoc).Rotation();

		//  파티클 시스템 스폰
		UGameplayStatics::SpawnEmitterAtLocation(
			GetWorld(),
			SwordParticleTemplate,
			MidPoint,
			Rotation,
			true
		);


	}
}

void UTraceComponent::PlayClangParticle()
{
	
	if (!SkeletalComp || !SwordParticleTemplate)
	{	
		UE_LOG(LogTemp, Error, TEXT("SkeletalComp or SwordClangParticleTemplate is null"));
		return;
	}

	for (const FTraceSockets& Socket : Sockets)
	{	
		const FVector StartLoc = SkeletalComp->GetSocketLocation(Socket.Start);
		const FVector EndLoc   = SkeletalComp->GetSocketLocation(Socket.End);

		const FVector MidPoint = (StartLoc + EndLoc) / 2.0f;
		const FRotator Rotation = (EndLoc - StartLoc).Rotation();

		//  파티클 시스템 스폰
		UGameplayStatics::SpawnEmitterAtLocation(
			GetWorld(),
			SwordParticleTemplate,
			MidPoint,
			Rotation,
			true
		);
	}


}

void UTraceComponent::EquipWeapon()
{	
	ACharacter* CharacterOwner = Cast<ACharacter>(GetOwner());

	if (!CharacterOwner || !SkeletalComp)
	{
		UE_LOG(LogTemp, Error, TEXT("CharacterOwner or SkeletalComp is null"));
		return;
	}
	SkeletalComp->AttachToComponent(
    CharacterOwner->GetMesh(),
    FAttachmentTransformRules::SnapToTargetIncludingScale,
    FName("weapon_r_posSocket") // 또는 "weapon_l_posSocket"
);

}
