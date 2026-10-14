// Fill out your copyright notice in the Description page of Project Settings.


#include "StatsComponent.h"
#include "EStat.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetSystemLibrary.h"
#include  "Fighter.h"

// Sets default values for this component's properties
UStatsComponent::UStatsComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UStatsComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	if (GetOwner())
	{
		if (UWorld* World = GetOwner()->GetWorld())
		{
			GameInstance = Cast<UMainGameInstance>(World->GetGameInstance());
		}
	}


	if (bDOTActive)
	{
		StartDOT();
	}


	//UE_LOG(LogTemp, Warning, TEXT("Health: %f"), Stats[EStat::Health] )
	
}


// Called every frame
void UStatsComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	/// DOT DAMAGE	

	// ...
}

void UStatsComponent::ReduceHealth(float Amount, AActor* Opponent)
{
	if (Stats[EStat::Health] <= 0)
	{
		return;
	}

	IFighter* FighterRef = GetOwner<IFighter>();

	if (!FighterRef->CanTakeDamage(Opponent))
	{
		return;
	}



	
	Stats[EStat::Health] -= Amount;

	Stats[EStat::Health] = UKismetMathLibrary::FClamp(Stats[EStat::Health], 0, Stats[EStat::MaxHealth]);


	OnHealthPercentUpdateDelegate.Broadcast(GetStatPercentage(EStat::Health, EStat::MaxHealth));

	if (Stats[EStat::Health] <= 0)
	{
		OnZeroHealthDelegate.Broadcast();
	}
}

void UStatsComponent::ReduceStamina(float Amount)
{
	
	Stats[EStat::Stamina] -= Amount;

	Stats[EStat::Stamina] = UKismetMathLibrary::FClamp(Stats[EStat::Stamina], 0, Stats[EStat::MaxStamina]);

	bCanRegen = false;

	FLatentActionInfo FunctionInfo
	{
		0,
		100,
		TEXT("EnableRegen"),
		this
	};

	UKismetSystemLibrary::RetriggerableDelay(GetWorld(), StaminaDelayDuration, FunctionInfo);

	OnStaminaPercentUpdateDelegate.Broadcast(GetStatPercentage(EStat::Stamina, EStat::MaxStamina));
}

void UStatsComponent::RegenStamina()
{	//TMap 안의 원소 ENUM 중 하나
	if (!bCanRegen)
	{
		return;
	}

	Stats[EStat::Stamina] = UKismetMathLibrary::FInterpTo_Constant(Stats[EStat::Stamina], Stats[EStat::MaxStamina], GetWorld()->DeltaTimeSeconds, StaminaRegenRate);

	OnStaminaPercentUpdateDelegate.Broadcast(GetStatPercentage(EStat::Stamina, EStat::MaxStamina));
}

void UStatsComponent::EnableRegen()
{
	bCanRegen = true;
}



float UStatsComponent::GetStatPercentage(EStat Current, EStat Max)
{
	return Stats[Current] / Stats[Max];
}

void UStatsComponent::IncreaseStrength(float Amount)
{
	Stats[EStat::Strength] += Amount;
}

void UStatsComponent::SetStrength(float Amount)
{
	Stats[EStat::Strength] = Amount;
}

float UStatsComponent::GetStrength()
{
	return Stats[EStat::Strength];
}

void UStatsComponent::SetMaxHealth(float Amount)
{
	Stats[EStat::MaxHealth] = Amount;
}




void UStatsComponent::AddHealth(float Amount)
{
	Stats[EStat::Health] += Amount;
	Stats[EStat::Health] = UKismetMathLibrary::FClamp(Stats[EStat::Health], 0, Stats[EStat::MaxHealth]);

	OnHealthPercentUpdateDelegate.Broadcast(GetStatPercentage(EStat::Health, EStat::MaxHealth));
}


///

void UStatsComponent::RegenHealth()
{
	// Maybe not using
}

void UStatsComponent::SetMaxStamina(float Amount)
{
	Stats[EStat::MaxStamina] += Amount;
}


/// DOT DAMGE
void UStatsComponent::GetSmallDamage(float Amount)
{

	if (Stats[EStat::Health] <= 0)
	{
		return;
	}

	// ReduceHealth는 상대/무적 체크 등 로직이 섞여 있을 수 있으니,
    // DOT은 ‘무조건 들어가는 지속 피해’라 가정하고 직접 처리 (클램프/브로드캐스트 포함)
    Stats[EStat::Health] -= Amount;
    Stats[EStat::Health] = UKismetMathLibrary::FClamp(Stats[EStat::Health], 0, Stats[EStat::MaxHealth]);

    OnHealthPercentUpdateDelegate.Broadcast(GetStatPercentage(EStat::Health, EStat::MaxHealth));
/*
    if (Stats[EStat::Health] <= 0)
    {
        //OnZeroHealthDelegate.Broadcast();
		// 델레게이트 남발 돼서 델레게이트 브로드케스트 계속 호출됨 -> 오우거 죽음처리 중복 오류

		return;
    }
*/
}

void UStatsComponent::StartDOT()
{
	bDOTActive = true;

	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().SetTimer(
			DotTimerHandle,
			this,
			&UStatsComponent::ApplyDOT_Interval,
			DOTInterval,
			true,
			DOTInterval
		);
	}
}

void UStatsComponent::StopDOT()
{
	bDOTActive = false;
	
	if(UWorld* World = GetWorld())
	{
		World->GetTimerManager().ClearTimer(DotTimerHandle);
	}
}

void UStatsComponent::ApplyDOT_Interval()
{
	if (!bDOTActive)
	{
		return;
	}

	GetSmallDamage(DOTDamagePerTick);
}



