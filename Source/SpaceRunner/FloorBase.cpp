// Fill out your copyright notice in the Description page of Project Settings.


#include "FloorBase.h"
#include "ScrollingComponent.h"
#include "LevelManager.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
AFloorBase::AFloorBase()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	ScrollingComponent = CreateDefaultSubobject<UScrollingComponent>(TEXT("ScrollingComponent"));
	AddOwnedComponent(ScrollingComponent);

	LevelManager = Cast<ALevelManager>(UGameplayStatics::GetActorOfClass(GetWorld(), ALevelManager::StaticClass()));

}

// Called when the game starts or when spawned
void AFloorBase::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AFloorBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

