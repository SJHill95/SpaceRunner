// Fill out your copyright notice in the Description page of Project Settings.


#include "ScrollingComponent.h"
#include "LevelManager.h"
#include "Kismet/GameplayStatics.h"

// Sets default values for this component's properties
UScrollingComponent::UScrollingComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UScrollingComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}

// Called every frame
void UScrollingComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	ALevelManager* LevelManager = Cast<ALevelManager>(UGameplayStatics::GetActorOfClass(GetWorld(), ALevelManager::StaticClass()));

	if (LevelManager)
	{
		Movement = GetOwner()->GetActorForwardVector() * LevelManager->GetLevelSpeed() * DeltaTime;
		GetOwner()->AddActorLocalOffset(Movement);
	}	
}

