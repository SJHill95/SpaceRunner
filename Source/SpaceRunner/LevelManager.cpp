// Fill out your copyright notice in the Description page of Project Settings.


#include "LevelManager.h"
#include "Components/BoxComponent.h"
#include "Blueprint/UserWidget.h"
#include "Kismet/GameplayStatics.h"
#include "SpaceRunnerCharacter.h"
#include "NiagaraComponent.h"


// Sets default values
ALevelManager::ALevelManager() :
	NumberOfLanes(3),
	LaneWidth(250.f),
	LevelSpeed(-500.f),
	MaxLevelSpeed(-1000.f),
	OutOfBoundBoxScale(32, 32, 32),
	bIsPlaying(false),
	bGameOver(false),
	Score(0.f),
	ScoreMultiplier(1.f)

{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	LevelBounds = CreateDefaultSubobject<UBoxComponent>(TEXT("LevelBounds"));
	LevelBounds->SetupAttachment(RootComponent);
	LevelBounds->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Overlap);
	LevelBounds->SetBoxExtent(OutOfBoundBoxScale, true);
	LevelBounds->SetWorldLocation(OutOfBoundBoxLocation);

}

// Called when the game starts or when spawned
void ALevelManager::BeginPlay()
{
	Super::BeginPlay();

	bIsPlaying = false;
	bGameOver = false;
	Materialize();

	PlayerCharRef = Cast<ASpaceRunnerCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));	

	/* Set up add score timer handle*/
	GetWorldTimerManager().SetTimer(AddScoreTimerHandle, this, &ALevelManager::AddScore, 0.1f, true, 0.5f);

	/* Set up increase level speed timer handle*/
	GetWorldTimerManager().SetTimer(IncreaseLevelSpeedTimerHandle, this, &ALevelManager::IncreaseLevelSpeed, 1.f, true, 1.f);
}

void ALevelManager::AddScore()
{
	float BaseScore{ 10.f };

	Score += BaseScore * ScoreMultiplier;

}

void ALevelManager::StopScore()
{
	GetWorldTimerManager().ClearTimer(AddScoreTimerHandle);
}

void ALevelManager::IncreaseLevelSpeed()
{
	switch (PlayerCharRef->CharacterState)
	{
		case ECharacterState::ECS_Running:
		break;
		case ECharacterState::ECS_Jumping:
		break;
		case ECharacterState::ECS_Sliding:
		break;
		case ECharacterState::ECS_Falling:
		break;
	}

	LevelSpeed, CurrentLevelSpeed = FMath::Clamp(LevelSpeed * 1.01f, MaxLevelSpeed, MinLevelSpeed);

	if (CurrentLevelSpeed == MaxLevelSpeed)
	{
		PlayerCharRef->GetSpeedLines()->SetVisibility(true);
	}
	else
	{
		PlayerCharRef->GetSpeedLines()->SetVisibility(false);
	}
}

// Called every frame
void ALevelManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}
/*
void ALevelManager::StartGame()
{
	if (bMaterializeFinished)
	{
		bIsPlaying = true;
	}

}
*/
void ALevelManager::GameOver()
{
	bGameOver = true;

	if (bGameOver)
	{
		LevelSpeed = 0.f;
		bIsPlaying = false;
		PlayerCharRef->StopOxygen();
		StopScore();

		if (GameOverWidgetAsset)
		{
			GameOverWidget = CreateWidget<UUserWidget>(UGameplayStatics::GetPlayerController(GetWorld(), 0), GameOverWidgetAsset);
			GameOverWidget->AddToViewport();
		}
	}	
}


