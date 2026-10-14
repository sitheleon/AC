#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Components/StaticMeshComponent.h"
#include "Fighter.h"
#include "Enemy.h"
#include "CableComponent.h"
#include "PerditCharacter.generated.h"

UCLASS()
class ACTIONCOMBAT_API APerditCharacter : public ACharacter, public IEnemy, public IFighter
{
	GENERATED_BODY()

private:
	UStaticMeshComponent* OrbMesh;
	FVector OrbStartLocation;
	float ElapsedTime = 0.0f;

	TArray<UStaticMeshComponent*> EyeMeshes;
	FVector StartLocation;
	float HoverElapsedTime = 0.0f;
	FVector OrbRelativeOffset = FVector(0.0f, 0.0f, 0.0f);

	void MoveOrb(float DeltaTime);
	void TemporarilyHideRandomEye(float HideDuration = 1.0f);
	void HoverMotion(float DeltaTime);



public:
	APerditCharacter();

protected:
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	class UStatsComponent* StatsComp;

	virtual float GetDamage() override;
	UFUNCTION(BlueprintCallable)
	void DetectPawn(APawn* DetectedPawn, APawn* PawnToDetect);

	UPROPERTY(EditAnywhere, Category = "Spawning")
	TSubclassOf<ACharacter> EnemyToSpawn;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Spawning")
	float SpawnInterval = 25.0f;

	FTimerHandle SpawnTimerHandle;

	UFUNCTION(BlueprintCallable, Category = "Spawning")
	void SpawnEnemy();



	UPROPERTY(EditAnywhere)
	class UMaterialInterface* CableMaterial;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Cable", meta = (AllowPrivateAccess = "true"))
	class UCableComponent* Thread;


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cable")
	class UMainGameInstance* GameInstance;

	// 현재 소환된 적을 추적
	UPROPERTY()
	ACharacter* CurrentSpawnedEnemy;

	// 적이 죽었을 때 호출될 함수
	UFUNCTION()
	void OnSpawnedEnemyDestroyed(AActor* DestroyedActor);


	//P2

	UFUNCTION(BlueprintCallable)
	void EndPhase2(UBehaviorTreeComponent* BTC);



	// hit

	UPROPERTY(EditAnywhere)
	float VibrateDuration = 0.22f;

	UPROPERTY(EditAnywhere)
	float VibrateAmplitude = 18.f;

	UPROPERTY(EditAnywhere)
	float KnockbackStrength = 250.f;

	bool bVibrating = false;
	float VibrateElapsed = 0.f;
	FVector VibrateOrigin;
	FVector VibrateDir = FVector::ZeroVector;

	FTimerHandle VibrateTimer;

	void VibrateTick();


	UFUNCTION(BlueprintCallable)
	void Vibrate();


	void Vibrate(const FVector& HitFromDirection, float StrengthScale = 1.f);

	UFUNCTION(BlueprintCallable)
	float GetHealth();

};