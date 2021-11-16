// Fill out your copyright notice in the Description page of Project Settings.


#include "Spawner.h"
#include "LevelManager.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
ASpawner::ASpawner()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	LevelManager = Cast<ALevelManager>(UGameplayStatics::GetActorOfClass(GetWorld(), ALevelManager::StaticClass()));
}

// Called when the game starts or when spawned
void ASpawner::BeginPlay()
{
	Super::BeginPlay();
	
}

AActor* ASpawner::SpawnActor_Implementation(TSubclassOf<AActor> ActorToSpawn)
{
	const FVector SpawnLoc = { 0, 0, -1000 };
	const FRotator Rotation = GetActorRotation();

	AActor* SpawnedActorRef = GetWorld()->SpawnActor<AActor>(ActorToSpawn, SpawnLoc, Rotation);

	return SpawnedActorRef;
}

void ASpawner::SpawnedActorHitBounds_Implementation(AActor* OutOfBoundsActor)
{

}

// Called every frame
void ASpawner::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}


