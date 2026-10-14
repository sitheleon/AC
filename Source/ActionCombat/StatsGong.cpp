// Fill out your copyright notice in the Description page of Project Settings.


#include "StatsGong.h"
#include "Components/StaticMeshComponent.h"
#include "Components/StaticMeshComponent.h"

// Sets default values
AStatsGong::AStatsGong()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	Mesh->SetupAttachment(RootComponent);

}

// Called when the game starts or when spawned
void AStatsGong::BeginPlay()
{
	Super::BeginPlay();

	APawn* PlayerPawn = GetWorld()->GetFirstPlayerController()->GetPawn();

	if (!PlayerPawn)
	{
		return;
	}

	AMainCharacter* MainChar = Cast<AMainCharacter>(PlayerPawn);

	if (!MainChar)
	{
		return;
	}

	//mainchar에 stats->로 접근해서 새 함수로 스텟 강화 로직 구현

	// wbp에서 케스팅 해서 버튼 이벤트 바인딩이 귀찮을지도
	// 그냥 mainchar에서 함수 구현.
}

// Called every frame
void AStatsGong::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

