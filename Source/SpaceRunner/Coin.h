// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/TimelineComponent.h"
#include "GameFramework/Actor.h"
#include "Coin.generated.h"

class UCurveFloat;

UCLASS()
class SPACERUNNER_API ACoin : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ACoin();

	// Called every frame
	virtual void Tick(float DeltaTime) override;
	
	UFUNCTION(BlueprintCallable)
	void MoveCoinsUp();

	UFUNCTION(BlueprintCallable)
	void MoveCoinsDown();

	UFUNCTION()
	void SetupTimeline();

	UFUNCTION()
	void MoveCoinsUpTimelineProgress(float Value);

	UFUNCTION()
	void MoveCoinsToPlayerTimelineProgress(float Value);

	//UFUNCTION(BlueprintImplementableEvent)
	UFUNCTION()
	void PickupCoin();

	UFUNCTION()
	void CoinProgressBarFilled();

	UFUNCTION()
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComp,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// Move coins up when flying timeline
	FTimeline MoveCoinsUpTimeline;

	UPROPERTY(EditAnywhere, Category = "Timeline")
	UCurveFloat* MoveCoinsUpFloat;

	// Move coins to play when picked up timeline
	FTimeline MoveCoinsToPlayerTimeline;

	UPROPERTY(EditAnywhere, Category = "Timeline")
	UCurveFloat* MoveCoinsToPlayerFloat;

	//Delegate signature for the function which will handle Finished event.
	FOnTimelineEvent TimelineFinished;

	UFUNCTION()
	void TimelineFinishedFunction();

	UPROPERTY()
	float StartLocZ;

	UPROPERTY(EditAnywhere, Category = "Timeline")
	float EndLocZ;

	UPROPERTY()
	FVector CoinLocation;

	UPROPERTY(EditAnywhere, Category = Sound)
	USoundBase* PickupSound;

public:	
	UPROPERTY(VisibleAnyWhere, BlueprintReadWrite, Category = "Coin")
	class UStaticMeshComponent* CoinMesh;

	UPROPERTY(VisibleAnyWhere, BlueprintReadWrite, Category = "Coin")
	class USphereComponent* OverlapSphere;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Default)
	float ScoreValue;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Default)
	float CoinsToLevel;

private: 

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Default, meta = (AllowPrivateAccess = "true"))
	class ASpaceRunnerCharacter* SpaceRunnerCharacter;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Default, meta = (AllowPrivateAccess = "true"))
	class ALevelManager* LevelManager;

	bool bCoinPickedUp;


public:

};
