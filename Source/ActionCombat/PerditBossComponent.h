// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"


#include "NiagaraSystem.h"
#include "NiagaraComponent.h"


#include "PerditBossComponent.generated.h"


// pattern enum
UENUM(BlueprintType)
enum class EPerdit2Pattern : uint8
{
	RangedTreads,
	XerathW,
	SealSkills
};


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class ACTIONCOMBAT_API UPerditBossComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UPerditBossComponent();

	UFUNCTION(BlueprintCallable)
	void StartPhase2Loop();

	UFUNCTION(BlueprintCallable)
	void StopPhase2Loop();


	// Called when the game starts
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;



private:
	// Loop

	FTimerHandle LoopHandle;
	bool bRunning = false;


	int32 LoopCount = 0;

	void LoopTick();

	//

	// AURA
	UPROPERTY(EditAnywhere, Category="Phase2|Aura")
	float AuraRadius = 450.f;

	UPROPERTY(EditAnywhere, Category="Phase2|Aura")
	float AuraDPS = 20.f;

	UPROPERTY(EditAnywhere, Category="Phase2|Aura")
	bool bDebugAura = true;

	UPROPERTY(EditAnywhere, Category="Phase2|Aura")
	TSubclassOf<UDamageType> AuraDamageType;

	void DealAuraDamage(float DeltaSeconds);

	// STAMINA
	UPROPERTY(EditAnywhere, Category="Phase2|Stamina")
	float MaxStamina = 100.f;

	UPROPERTY(EditAnywhere, Category="Phase2|Stamina")
	float StaminaRegenPerSec = 20.f;

	UPROPERTY(EditAnywhere, Category="Phase2|Stamina")
	float GroggyDuration = 4.0f;

	UFUNCTION(BlueprintCallable)
	float GetStamina() const;

	UFUNCTION(BlueprintCallable)
	bool IsGroggy() const;

	UFUNCTION(BlueprintCallable)
	void ConsumeStamina(float Amount);

	float Stamina = 0.f;
	bool bGroggy = false;
	FTimerHandle GroggyHandle;

	void EnterGroggy();
	void ExitGroggy();


	// phase 2 pattern
	UPROPERTY(EditAnywhere, Category = "Phase|Pattern")
	float PatternGapMin = 0.8f;

	UPROPERTY(EditAnywhere, Category = "Phase|Pattern")
	float PatternGapMAx = 1.6f;

	UPROPERTY(EditAnywhere, Category = "Phase|Pattern|CD")
	float RangedThreadsCD = 5.0f;

	UPROPERTY(EditAnywhere, Category = "Phase|Pattern|CD")
	float XerathWCD = 6.0f;

	UPROPERTY(EditAnywhere, Category = "Phase|Pattern|CD")
	float SealSkillCD = 8.0f;


	float NextAvail_Ranged = 0.f;
	float NextAvail_Xerath = 0.f;
	float NextAvail_Seal = 0.f;

	// ENUM
	EPerdit2Pattern SelectNextPattern() const;
	bool IsOnCoolDown(EPerdit2Pattern P, float New) const;
	void ArmCooldown(EPerdit2Pattern P, float New);
	void RunPattern(EPerdit2Pattern P);



	//Projectile
	UPROPERTY(EditAnywhere, Category="Phase2|Assets")
	TSubclassOf<AActor> ThreadProjectileClass;

	void DoRangedThreads();



	//Xerah W
	void Do_XerathW();


	//vfx
	UPROPERTY(EditAnywhere, Category="Phase2|VFX")
	UNiagaraSystem* NS_Aura;

	UPROPERTY(EditAnywhere, Category="Phase2|VFX")
	UNiagaraSystem* NS_XerathMarker = nullptr;

	UPROPERTY(EditAnywhere, Category="Phase2|VFX")
	UNiagaraSystem* NS_XerathExplosion = nullptr; // 선택

	// 런타임 핸들(자기장 상시용)
	UPROPERTY(Transient)
	UNiagaraComponent* AuraComp = nullptr;

	void ActivateAuraVFX();
	void UpdateAuraRadius(float NewRadius);
	void DeactivateAuraVFX();

	static void SafeDestroyNiagara(UWorld* W, UNiagaraComponent* Comp, float DelaySec = 0.f);



	// 사운드
	UPROPERTY(EditAnywhere)
	USoundBase* ThreadSound = nullptr;



		
};
