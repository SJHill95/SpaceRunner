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

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void GameOver();

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintImplementableEvent)
	void StartGame();

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void PauseLevelSpeed();

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void UnpauseLevelSpeed();

private:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Lanes, meta = (AllowPrivateAccess = "true"))
	int32 NumberOfLanes;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Lanes, meta = (AllowPrivateAccess = "true"))
	float LaneWidth;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Speed, meta = (AllowPrivateAccess = "true"))
	float LevelSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Speed, meta = (AllowPrivateAccess = "true"))
	float CurrentLevelSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Bounds, meta = (AllowPrivateAccess = "true"))
	FVector OutOfBoundBoxScale;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Bounds, meta = (AllowPrivateAccess = "true", MakeEditWidget = true))
	FVector OutOfBoundBoxLocation;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Bounds, meta = (AllowPrivateAccess = "true", MakeEditWidget = true))
	class UBoxComponent* LevelBounds;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Default, meta = (AllowPrivateAccess = "true"))
	bool bIsPlaying;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Score, meta = (AllowPrivateAccess = "true"))
	int32 Score;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = StartGame, meta = (AllowPrivateAccess = "true"))
	bool bMaterializeFinished;

public:

	FORCEINLINE int32 GetNumberOfLanes() const { return NumberOfLanes; }
	FORCEINLINE float GetLaneWidth() const { return LaneWidth; }
	FORCEINLINE float GetLevelSpeed() const { return LevelSpeed; }
	FORCEINLINE float GetCurrentLevelSpeed() const { return CurrentLevelSpeed; }
	FORCEINLINE bool GetIsPlaying() const { return bIsPlaying; }
	FORCEINLINE int32 GetScore() const { return Score; }

	FORCEINLINE void SetScore(int32 ScoreAmount) { Score = ScoreAmount; }
	FORCEINLINE void SetLevelSpeed(float NewLevelSpeed) { LevelSpeed = NewLevelSpeed; }
};
