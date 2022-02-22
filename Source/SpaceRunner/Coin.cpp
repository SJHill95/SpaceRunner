// Fill out your copyright notice in the Description page of Project Settings.


#include "Coin.h"
#include "Components/TimelineComponent.h"
#include "Kismet/GameplayStatics.h"
#include "SpaceRunnerCharacter.h"
#include "LevelManager.h"
#include "Obstacle.h"
#include "Components/SphereComponent.h"

// Sets default values
ACoin::ACoin() :
	StartLocZ(100.f),
	EndLocZ(400.f),
	ScoreValue(50)

{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	CoinMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("CoinMesh"));
	CoinMesh->SetupAttachment(GetRootComponent());

	OverlapSphere = CreateDefaultSubobject<USphereComponent>(TEXT("OverlapSphere"));
	OverlapSphere->SetupAttachment(CoinMesh);

}

// Called when the game starts or when spawned
void ACoin::BeginPlay()
{
	Super::BeginPlay();

	SpaceRunnerCharacter = Cast<ASpaceRunnerCharacter>(UGameplayStatics::GetActorOfClass(GetWorld(), ASpaceRunnerCharacter::StaticClass()));
	LevelManager = Cast<ALevelManager>(UGameplayStatics::GetActorOfClass(GetWorld(), ALevelManager::StaticClass()));

	SetupTimeline();

	OverlapSphere->OnComponentBeginOverlap.AddDynamic(this, &ACoin::OnOverlapBegin);
	
	if (SpaceRunnerCharacter->CharacterState == ECharacterState::ECS_Flying)
	{
		MoveCoinsUp();
	}

}

// Called every frame
void ACoin::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// Timelines
	MoveCoinsUpTimeline.TickTimeline(DeltaTime);
	MoveCoinsToPlayerTimeline.TickTimeline(DeltaTime);

	/*
	
	*/
}

void ACoin::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	AActor* PlayerCharacter = Cast<ASpaceRunnerCharacter>(OtherActor);
	AActor* Obstacle = Cast<AObstacle>(OtherActor);

	if (this != nullptr)
	{
		if (PlayerCharacter)
		{
			PickupCoin();
		}
		//Cast<ASpaceRunnerCharacter>(OtherActor)->DisplayCoinWidget();

		if (Obstacle)
		{
			Destroy();
			UE_LOG(LogTemp, Warning, TEXT("Coin Destroyed"));
		}
	}	
}

void ACoin::SetupTimeline()
{
	if (MoveCoinsUpFloat)
	{
		FOnTimelineFloat MoveCoinsUpTimelineProgress;
		MoveCoinsUpTimelineProgress.BindUFunction(this, FName("MoveCoinsUpTimelineProgress"));
		MoveCoinsUpTimeline.AddInterpFloat(MoveCoinsUpFloat, MoveCoinsUpTimelineProgress);
	}

	if (MoveCoinsToPlayerFloat)
	{
		FOnTimelineFloat MoveCoinsToPlayerTimelineProgress;
		MoveCoinsToPlayerTimelineProgress.BindUFunction(this, FName("MoveCoinsToPlayerTimelineProgress"));
		TimelineFinished.BindUFunction(this, FName("TimelineFinishedFunction"));
		MoveCoinsToPlayerTimeline.AddInterpFloat(MoveCoinsToPlayerFloat, MoveCoinsToPlayerTimelineProgress);
	}
}

void ACoin::MoveCoinsUpTimelineProgress(float Value)
{	 
	float NewZLoc = FMath::Lerp(StartLocZ, EndLocZ, Value);
	
	CoinLocation = { GetActorLocation().X, GetActorLocation().Y, NewZLoc };

	SetActorLocation(CoinLocation);
}

void ACoin::MoveCoinsToPlayerTimelineProgress(float Value)
{
	FVector PlayerEndLocation = { SpaceRunnerCharacter->GetActorLocation().X,SpaceRunnerCharacter->GetActorLocation().Y, SpaceRunnerCharacter->GetActorLocation().Z + 150 };

	FVector NewCoinLocation = FMath::Lerp(GetActorLocation(), PlayerEndLocation, Value);

	SetActorLocation(NewCoinLocation);
}

void ACoin::TimelineFinishedFunction()
{
	UE_LOG(LogTemp, Warning, TEXT("Finished Event Called."));
	SpaceRunnerCharacter->SetCoins(SpaceRunnerCharacter->GetCoins() + 1);
	SpaceRunnerCharacter->SetCoinsTotal(SpaceRunnerCharacter->GetCoinsTotal() + 1);
	UGameplayStatics::PlaySound2D(this, PickupSound);
	LevelManager->SetScore(LevelManager->GetScore() + ScoreValue);
	SpaceRunnerCharacter->DisplayCoinWidget();

	CoinMesh->SetVisibility(false);
}

void ACoin::PickupCoin()
{
	MoveCoinsToPlayerTimeline.Play();

	SetActorScale3D({ 0.5f, 0.5f, 0.5f });

	MoveCoinsToPlayerTimeline.SetTimelineFinishedFunc(TimelineFinished);

	if (SpaceRunnerCharacter->GetCoins() >= 99)
	{
		CoinProgressBarFilled();
	}
	
}

void ACoin::CoinProgressBarFilled()
{
	SpaceRunnerCharacter->SetCoins(0);
	SpaceRunnerCharacter->CoinBarFilled();
	LevelManager->SetScore(LevelManager->GetScore() + 1000);
}

void ACoin::MoveCoinsUp()
{
	MoveCoinsUpTimeline.Play();
}

void ACoin::MoveCoinsDown()
{
	MoveCoinsUpTimeline.Reverse();
}
