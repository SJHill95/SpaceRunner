// Fill out your copyright notice in the Description page of Project Settings.


#include "LevelManager.h"
#include "Components/BoxComponent.h"
#include "Blueprint/UserWidget.h"
#include "Kismet/GameplayStatics.h"
#include "SpaceRunnerCharacter.h"
#include "NiagaraComponent.h"
#include "Math/UnrealMathUtility.h"


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
	ScoreMultiplier(1.f),
	bMeteorShower(false),
	MSDurationMin(15.f),
	MSDurationMax(30.f),
	MSCountdown(60.f),
	MSCountdownCurrent(0.f),
	bShowHUD(false)

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
	bMeteorShower = false;
	Materialize();

	PlayerCharRef = Cast<ASpaceRunnerCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));	

	/* Set up add score timer handle*/
	GetWorldTimerManager().SetTimer(AddScoreTimerHandle, this, &ALevelManager::AddScore, 0.1f, true, 0.5f);

	/* Set up increase level speed timer handle*/
	GetWorldTimerManager().SetTimer(IncreaseLevelSpeedTimerHandle, this, &ALevelManager::IncreaseLevelSpeed, 1.f, true, 1.f);

	MSCountdownCurrent = MSCountdown;
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
	if (bIsPlaying)
	{
		if (PlayerCharRef->CharacterState == ECharacterState::ECS_Running ||
			PlayerCharRef->CharacterState == ECharacterState::ECS_Jumping ||
			PlayerCharRef->CharacterState == ECharacterState::ECS_Sliding ||
			PlayerCharRef->CharacterState == ECharacterState::ECS_Falling)
		{
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
	}
	
}

void ALevelManager::StopFloorMovement()
{
	LevelSpeed = 0;
	bIsPlaying = false;
	StopScore();
	GetWorldTimerManager().PauseTimer(LevelSpeedTimerHandle);
}

void ALevelManager::StartFloorMovement()
{
	LevelSpeed = CurrentLevelSpeed;
	bIsPlaying = true;
	AddScore();
	GetWorldTimerManager().UnPauseTimer(LevelSpeedTimerHandle);
}

// Called every frame
void ALevelManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ALevelManager::StartGame()
{
	if (bMaterializeFinished)
	{
		bIsPlaying = true;
		GetWorldTimerManager().SetTimer(LevelSpeedTimerHandle, this, &ALevelManager::IncreaseLevelSpeed, 1.f, true, 0.f);
		GetWorldTimerManager().SetTimer(MSCountdownTimerHandle, this, &ALevelManager::CountdownToMeteorShower, 1.f, true, 0.f);
		SetupGame();
		bShowHUD = true;
	}

}

void ALevelManager::GameOver()
{
	bGameOver = true;

	if (bGameOver)
	{
		LevelSpeed = 0.f;
		bIsPlaying = false;
		PlayerCharRef->StopOxygen();
		StopScore();
		bShowHUD = false;
		HideHUD();
		if (GameOverWidgetAsset)
		{
			GameOverWidget = CreateWidget<UUserWidget>(UGameplayStatics::GetPlayerController(GetWorld(), 0), GameOverWidgetAsset);
			GameOverWidget->AddToViewport();
		}
	}	
}

void ALevelManager::MeteorShower()
{
	float MSDuration = FMath::RandRange(MSDurationMin, MSDurationMax);

	if (!bMeteorShower)
	{
		if (GEngine)
			GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, TEXT("Meteor Shower Started!"));

		bMeteorShower = true;
		GetWorldTimerManager().SetTimer(MSTimerHandle, this, &ALevelManager::StopMeteorShower, 1.f, false, MSDuration);
		MSCountdownCurrent = MSCountdown;
		
	}
}

void ALevelManager::StopMeteorShower()
{
	if (bMeteorShower)
	{
		if (GEngine)
			GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Blue, TEXT("Meteor Shower Stopped!"));

		bMeteorShower = false;
		GetWorldTimerManager().SetTimer(MSCountdownTimerHandle, this, &ALevelManager::CountdownToMeteorShower, 1.f, true, 0.f);
	}
}

void ALevelManager::CountdownToMeteorShower()
{
	if (!bMeteorShower)
	{
		MSCountdownCurrent--;

		if (MSCountdownCurrent <= 0)
		{
			GetWorldTimerManager().ClearTimer(MSCountdownTimerHandle);
			MeteorShower();
		}
	}
}


