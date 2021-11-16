// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Spawner.generated.h"

UCLASS()
class SPACERUNNER_API ASpawner : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASpawner();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	class AActor* SpawnActor(TSubclassOf<AActor> ActorToSpawn);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void SpawnedActorHitBounds(AActor* OutOfBoundsActor);


public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

private:

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = default, meta = (AllowPrivateAccess = "true"))
	TSubclassOf<AActor>SpawnActorType;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = default, meta = (AllowPrivateAccess = "true"))
	int32 ActorsRequiredCount;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Default, meta = (AllowPrivateAccess = "true"))
	class ALevelManager* LevelManager;

};
