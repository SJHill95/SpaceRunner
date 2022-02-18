// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "SpaceRunnerCharacter.generated.h"

UENUM(BlueprintType)
enum class ECharacterType : uint8
{
	ECT_Runner UMETA(DisplayName="Runner"),
	ECT_Freeroam UMETA(DisplayName = "Freeroam")
};

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

class UNiagaraComponent;

UCLASS(config=Game)
class ASpaceRunnerCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	ASpaceRunnerCharacter();

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	ECharacterType CharacterType;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	ECharacterState CharacterState;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bIsRunnerCharacter;

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

	// Flying functions
	void StartFlying();
	void StopFlying();

	// Move Down and Sliding functions
	void MoveDown();
	void Slide();
	void StopSliding();

public:

	void RestartLevel();

	UFUNCTION()
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComp,
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

private:
	// Camera boom positioning the camera behind the character
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* CameraBoom;

	// Camera that follows the character
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* FollowCamera;

	UPROPERTY(VisibleAnyWhere, BlueprintReadOnly, Category = "Magnet", meta = (AllowPrivateAccess = "true"))
	class USphereComponent* MagnetSphereCollision;

	float zPosition;
	FVector tempPos = FVector();

	bool CanMove;

	float JumpVelocity;

	float GravityScale;

	float CharacterSpeed;

	// Flying variables
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Flying, meta = (AllowPrivateAccess = "true"))
	UNiagaraComponent* JetpackParticles;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Flying, meta = (AllowPrivateAccess = "true"))
	UNiagaraComponent* SpeedLines;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Flying, meta = (AllowPrivateAccess = "true"))
	class USkeletalMeshComponent* JetpackMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Flying, meta = (AllowPrivateAccess = "true"))
	class USoundBase* JetpackSFX;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Flying, meta = (AllowPrivateAccess = "true"))
	float FlyingHeightZ;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Flying, meta = (AllowPrivateAccess = "true"))
	float FlyingSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Flying, meta = (AllowPrivateAccess = "true"))
	bool bIsFlying;

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

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Default, meta = (AllowPrivateAccess = "true"))
	class ALevelManager* LevelManager;

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
};