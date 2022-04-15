// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Components/TimelineComponent.h"
#include "SpaceRunnerCharacter.generated.h"


UENUM(BlueprintType)
enum class ECharacterState : uint8
{
	ECS_Running UMETA(DisplayName = "Running"),
	ECS_Stationary UMETA(DisplayName = "Stationary"),
	ECS_Jumping UMETA(DisplayName = "Jumping"),
	ECS_Sliding UMETA(DisplayName = "Sliding"),
	ECS_Flying UMETA(DisplayName = "Flying"),
	ECS_Falling UMETA(DisplayName = "Falling"),
	ECS_Dead UMETA(DisplayName = "Dead")
};

USTRUCT(BlueprintType)
struct FQuizStruct
{
	GENERATED_BODY();

public:

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FText Question;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FText Answer;
};

class UNiagaraComponent;
class UUserWidget;

UCLASS(config=Game)
class ASpaceRunnerCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	ASpaceRunnerCharacter();

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	ECharacterState CharacterState;
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UFUNCTION()
	void SetupTimeline();

	UFUNCTION()
	void MoveLeftTimelineProgress(float Value);

	UFUNCTION()
	void TimelineFinishedFunction();

	void RestartLevel();

	UFUNCTION()
	void OnOverlapBeginMagnet(UPrimitiveComponent* OverlappedComp,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult);

	UFUNCTION()
	void OnOverlapBeginForceField(UPrimitiveComponent* OverlappedComp,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult);

	UFUNCTION(BlueprintImplementableEvent)
	void DisplayCoinWidget();

	UFUNCTION(BlueprintImplementableEvent)
	void CoinBarFilled();

	UFUNCTION()
	void StartGame();

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void FlyingTurnRotation(FRotator Rotation);

	void DecreaseOxygen();

	UFUNCTION(BlueprintCallable)
	void RestoreOxygen();

	UFUNCTION(BlueprintCallable)
	void StopOxygen();

	UFUNCTION(BlueprintCallable)
	void QuizPickup();

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void CheckPlayerLane();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void CheckPlayerAnswer();

protected:
	void MoveRight(float value);

	void Jump();
	// Sets up the lanes
	void Initialize();

	UFUNCTION()
	void MoveLeft();
	UFUNCTION()
	void MoveRight();

	// Flying functions
	UFUNCTION(BlueprintCallable)
	void StartFlying();
	UFUNCTION(BlueprintCallable)
	void StopFlying();

	// Move Down and Sliding functions
	void MoveDown();
	void Slide();
	void StopSliding();

	void ESCDown();

	// Move left timeline 
	FTimeline MoveLeftTimeline;

	UPROPERTY(EditAnywhere, Category = "Timeline")
	UCurveFloat* MoveLeftFloat;

	//Delegate signature for the function which will handle Finished event.
	FOnTimelineEvent TimelineFinished;

	// Pickups
	UFUNCTION(BlueprintCallable)
	void MagnetOn();

	UFUNCTION(BlueprintCallable)
	void MagnetOff();

	UFUNCTION(BlueprintCallable)
	void ForceFieldOn();

	UFUNCTION(BlueprintCallable)
	void ForceFieldOff();

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void JetpackOn();

public:

	FTimerHandle DecreaseOxygenTimerHandle;

	FTimerHandle RestoreOxygenTimerHandle;

	FTimerHandle QuizTimerHandle;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Default)
	int32 Lives;

private:
	// Camera boom positioning the camera behind the character
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* CameraBoom;

	// Camera that follows the character
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* FollowCamera;

	UPROPERTY(VisibleAnyWhere, BlueprintReadOnly, Category = "Magnet", meta = (AllowPrivateAccess = "true"))
	class USphereComponent* MagnetSphereCollision;

	// Force Field variables

	UPROPERTY(VisibleAnyWhere, BlueprintReadOnly, Category = "ForceField", meta = (AllowPrivateAccess = "true"))
	class USphereComponent* ForceFieldSphereCollision;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ForceField", meta = (AllowPrivateAccess = "true"))
	UNiagaraComponent* ForceFieldFX;
	
	UPROPERTY(VisibleAnyWhere, BlueprintReadOnly, Category = "ForceField", meta = (AllowPrivateAccess = "true"))
	bool bForceFieldOn;

	// Movement Variables
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Movement, meta = (AllowPrivateAccess = "true"))
	bool bMoving;

	bool bIsTurningLeft;

	bool bIsTurningRight;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Movement, meta = (AllowPrivateAccess = "true"))
	float JumpVelocity;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Movement, meta = (AllowPrivateAccess = "true"))
	float GravityScale;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Movement, meta = (AllowPrivateAccess = "true"))
	float CharacterSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Movement, meta = (AllowPrivateAccess = "true"))
	class UAnimMontage* RunningTurnLeft;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Movement, meta = (AllowPrivateAccess = "true"))
	class UAnimMontage* RunningTurnRight;

	// Flying variables
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Flying, meta = (AllowPrivateAccess = "true"))
	UNiagaraComponent* JetpackParticles;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Flying, meta = (AllowPrivateAccess = "true"))
	UNiagaraComponent* SpeedLines;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Flying, meta = (AllowPrivateAccess = "true"))
	class USkeletalMeshComponent* JetpackMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Flying, meta = (AllowPrivateAccess = "true"))
	class UAudioComponent* JetpackSFX;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Flying, meta = (AllowPrivateAccess = "true"))
	float FlyingHeightZ;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Flying, meta = (AllowPrivateAccess = "true"))
	float FlyingSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Flying, meta = (AllowPrivateAccess = "true"))
	bool bIsFlying;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Flying, meta = (AllowPrivateAccess = "true"))
	FRotator FlyingTurnRotationValue;

	// Coin variables
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Coins, meta = (AllowPrivateAccess = "true"))
	int32 Coins;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Coins, meta = (AllowPrivateAccess = "true"))
	int32 CoinsTotal;

	// Lane variables
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Bounds, meta = (AllowPrivateAccess = "true", MakeEditWidget = true))
	int32 NumberOfLanes;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Bounds, meta = (AllowPrivateAccess = "true", MakeEditWidget = true))
	float LaneWidth;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Bounds, meta = (AllowPrivateAccess = "true", MakeEditWidget = true))
	int32 CurrentLane;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Bounds, meta = (AllowPrivateAccess = "true", MakeEditWidget = true))
	float TargetPosition;

	// Player level variables
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Level, meta = (AllowPrivateAccess = "true"))
	int32 CurrentPlayerLevel;

	/* Oxygen variables */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Oxygen", meta = (AllowPrivateAccess = "true"))
	float Oxygen;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Oxygen", meta = (AllowPrivateAccess = "true"))
	float MaxOxygen;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Oxygen", meta = (AllowPrivateAccess = "true"))
	float RestoreOxygenAmount;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Oxygen", meta = (AllowPrivateAccess = "true"))
	UNiagaraComponent* OxygenRestoreFX;

	/* Quiz pickup variables */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Quiz, meta = (AllowPrivateAccess = "true"))
	TSubclassOf<class UUserWidget> QuizWidgetAsset;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Quiz, meta = (AllowPrivateAccess = "true"))
	UUserWidget* QuizWidget;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Quiz, meta = (AllowPrivateAccess = "true"))
	FText PlayerAnswer;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Quiz, meta = (AllowPrivateAccess = "true"))
	bool bQuizIsActive;

	/* Reference to the level manager */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Default, meta = (AllowPrivateAccess = "true"))
	class ALevelManager* LevelManager;

	/* Reference to the coin class */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Default, meta = (AllowPrivateAccess = "true"))
	class ACoin* CoinRef;

	/* Reference to the player controller */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Default, meta = (AllowPrivateAccess = "true"))
	class ARunnerPlayerController* RunnerPlayerController;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Default, meta = (AllowPrivateAccess = "true"))
	bool bAllowPlayerInput;

public:
	// Returns CameraBoom subobject
	FORCEINLINE USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	// Returns follow camera suboject
	FORCEINLINE UCameraComponent* GetFollowCamera() const { return FollowCamera; }

	FORCEINLINE bool GetIsFlying() { return bIsFlying; }

	// Coins
	FORCEINLINE int32 GetCoins() { return Coins; }
	FORCEINLINE void SetCoins(float CoinsAmount) { Coins = CoinsAmount; }
	FORCEINLINE int32 GetCoinsTotal() { return CoinsTotal; }
	FORCEINLINE void SetCoinsTotal(float CoinsAmount) { CoinsTotal = CoinsAmount; }

	FORCEINLINE int32 GetCurrentPlayerLevel() { return CurrentPlayerLevel; }

	FORCEINLINE UNiagaraComponent* GetSpeedLines() { return SpeedLines; }
};