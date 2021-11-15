// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "LevelManager.generated.h"

UCLASS()
class SPACERUNNER_API ALevelManager : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ALevelManager();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

private:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Lanes, meta = (AllowPrivateAccess = "true"))
		int32 NumberOfLanes;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Lanes, meta = (AllowPrivateAccess = "true"))
		float LaneWidth;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Speed, meta = (AllowPrivateAccess = "true"))
		float LevelSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Bounds, meta = (AllowPrivateAccess = "true"))
		FVector OutOfBoundBoxScale;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Bounds, meta = (AllowPrivateAccess = "true", MakeEditWidget = true))
		FVector OutOfBoundBoxLocation;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Bounds, meta = (AllowPrivateAccess = "true", MakeEditWidget = true))
		class UBoxComponent* LevelBounds;

public:

	FORCEINLINE int32 GetNumberOfLanes() const { return NumberOfLanes; }
	FORCEINLINE float GetLaneWidth() const { return LaneWidth; }
	FORCEINLINE float GetLevelSpeed() const { return LevelSpeed; }
};
