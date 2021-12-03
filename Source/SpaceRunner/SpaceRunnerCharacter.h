// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "SpaceRunnerCharacter.generated.h"


UCLASS(config=Game)
class ASpaceRunnerCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	ASpaceRunnerCharacter();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

protected:
	void MoveRight(float value);

	void Jump();
	// Sets up the lanes
	void Initialize();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
		void MoveLeft();
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
		void MoveRight();

public:

	void RestartLevel();

	
	UFUNCTION()
		void OnOverlapBegin(UPrimitiveComponent* OverlappedComp,
			AActor* OtherActor,
			UPrimitiveComponent* OtherComp,
			int32 OtherBodyIndex,
			bool bFromSweep,
			const FHitResult& SweepResult);
private:
	// Camera boom positioning the camera behind the character
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
		class USpringArmComponent* CameraBoom;

	// Camera that follows the character
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
		class UCameraComponent* FollowCamera;

	float zPosition;
	FVector tempPos = FVector();

	bool CanMove;

	float JumpVelocity;

	float GravityScale;

	float CharacterSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Default, meta = (AllowPrivateAccess = "true"))
	bool bIsFlying;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Bounds, meta = (AllowPrivateAccess = "true", MakeEditWidget = true))
		int32 NumberOfLanes;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Bounds, meta = (AllowPrivateAccess = "true", MakeEditWidget = true))
		float LaneWidth;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Bounds, meta = (AllowPrivateAccess = "true", MakeEditWidget = true))
		int32 CurrentLane;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Bounds, meta = (AllowPrivateAccess = "true", MakeEditWidget = true))
		float TargetPosition;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Default, meta = (AllowPrivateAccess = "true"))
		class ALevelManager* LevelManager;

public:
	// Returns CameraBoom subobject
	FORCEINLINE USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	// Returns follow camera suboject
	FORCEINLINE UCameraComponent* GetFollowCamera() const { return FollowCamera; }
};