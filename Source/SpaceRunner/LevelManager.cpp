// Fill out your copyright notice in the Description page of Project Settings.


#include "LevelManager.h"
#include "Components/BoxComponent.h"

// Sets default values
ALevelManager::ALevelManager() :
NumberOfLanes(3),
LaneWidth(250.f),
LevelSpeed(-500.f),
OutOfBoundBoxScale(32, 32, 32),
bIsPlaying(false),
Score(0)

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
void ALevelManager::GameOver_Implementation()
{

}

