// Fill out your copyright notice in the Description page of Project Settings.


#include "SpaceRunnerCharacter.h"
#include "Components/CapsuleComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "LevelManager.h"
#include "Obstacle.h"
#include "EngineUtils.h"
#include "Kismet/GameplayStatics.h"
#include "Components/WidgetComponent.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Coin.h"
#include "NiagaraComponent.h"
#include "Components/AudioComponent.h"
#include "RunnerPlayerController.h"
#include "Animation/AnimMontage.h"
#include "Components/TimelineComponent.h"
#include "Blueprint/UserWidget.h"

// Sets default values
ASpaceRunnerCharacter::ASpaceRunnerCharacter() :
	// Jumping variables
	JumpVelocity(600.f),
	GravityScale(1.f),
	// Movement
	CharacterSpeed(600.f),
	// Coins
	Coins(0.f),
	CoinsTotal(0.f),
	// Flying
	FlyingHeightZ(400.f),
	bIsFlying(false),
	FlyingSpeed(-2000.f),
	FlyingTurnRotationValue({0.f, 0.f, 0.f}),
	// Player Level
	CurrentPlayerLevel(1),
	// Oxygen
	Oxygen(100.f),
	MaxOxygen(100.f),
	RestoreOxygenAmount(0.05f)
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Capsule size and collision
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.f);
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_GameTraceChannel1, ECR_Overlap);

	bUseControllerRotationPitch = false;
	bUseControllerRotationRoll = false;
	bUseControllerRotationYaw = false;

	// Create camera boom
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 180.f;
	//CameraBoom->bUsePawnControlRotation = true;
	CameraBoom->SocketOffset = FVector(0.f, 50.f, 70.f);

	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	FollowCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

	// Character starting rotation
	//GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.f, 720.f, 0.f);

	// Character movement
	GetCharacterMovement()->GravityScale = GravityScale;
	GetCharacterMovement()->AirControl = 0.8f;
	GetCharacterMovement()->JumpZVelocity = JumpVelocity;
	GetCharacterMovement()->GroundFriction = 3.f;
	GetCharacterMovement()->MaxWalkSpeed = CharacterSpeed;
	GetCharacterMovement()->MaxFlySpeed = CharacterSpeed;

	// Magnet Collision
	MagnetSphereCollision = CreateDefaultSubobject<USphereComponent>(TEXT("MagnetSphereCollision"));
	MagnetSphereCollision->SetupAttachment(RootComponent);

	//Force Field Collision and Niagara
	ForceFieldSphereCollision = CreateDefaultSubobject<USphereComponent>(TEXT("ForceFieldSphereCollision"));
	ForceFieldSphereCollision->SetupAttachment(RootComponent);

	ForceFieldFX = CreateDefaultSubobject<UNiagaraComponent>(TEXT("ForceFieldFX"));
	ForceFieldFX->SetupAttachment(ForceFieldSphereCollision);

	// Jetpack/Flying Particles and mesh
	JetpackMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("JetpackMesh"));

	JetpackParticles = CreateDefaultSubobject<UNiagaraComponent>(TEXT("JetpackParticles"));
	JetpackParticles->SetupAttachment(JetpackMesh);

	SpeedLines = CreateDefaultSubobject<UNiagaraComponent>(TEXT("SpeedLines"));
	SpeedLines->SetupAttachment(RootComponent);

	// Oxygen FX
	OxygenRestoreFX = CreateDefaultSubobject<UNiagaraComponent>(TEXT("OxygenRestoreFX"));
	OxygenRestoreFX->SetupAttachment(RootComponent);

}

// Called when the game starts or when spawned
void ASpaceRunnerCharacter::BeginPlay()
{
	Super::BeginPlay();

	// Get Actor References
	LevelManager = Cast<ALevelManager>(UGameplayStatics::GetActorOfClass(GetWorld(), ALevelManager::StaticClass()));
	CoinRef = Cast<ACoin>(UGameplayStatics::GetActorOfClass(GetWorld(), ACoin::StaticClass()));
	RunnerPlayerController = Cast<ARunnerPlayerController>(GetController());

	// Set up Lanes
	Initialize();

	// Set up collisions
	MagnetSphereCollision->OnComponentBeginOverlap.AddDynamic(this, &ASpaceRunnerCharacter::OnOverlapBeginMagnet);
	ForceFieldSphereCollision->OnComponentBeginOverlap.AddDynamic(this, &ASpaceRunnerCharacter::OnOverlapBeginForceField);

	SetupTimeline();

	// Setup Timers
	GetWorldTimerManager().SetTimer(DecreaseOxygenTimerHandle, this, &ASpaceRunnerCharacter::DecreaseOxygen, 0.01f, true, 1.0f);


}

void ASpaceRunnerCharacter::Initialize()
{
	if (LevelManager)
	{
		NumberOfLanes = LevelManager->GetNumberOfLanes();
		LaneWidth = LevelManager->GetLaneWidth();
		CurrentLane = NumberOfLanes / 2 + 1;
		UE_LOG(LogTemp, Warning, TEXT("number of lanes: %d"), LevelManager->GetNumberOfLanes());
	}
}

void ASpaceRunnerCharacter::MoveLeft()
{
	if (LevelManager->GetIsPlaying())
	{
		if (CharacterState == ECharacterState::ECS_Flying || CharacterState == ECharacterState::ECS_Running)
		{
			if (!bMoving)
			{
				if (!(CurrentLane <= 1))
				{
					bMoving = true;
					bIsTurningLeft = true;
					TargetPosition = GetActorLocation().Y - LaneWidth;
					FlyingTurnRotationValue = { 0.f ,0.f,-30.f };

					if (CharacterState == ECharacterState::ECS_Running)
					{
						PlayAnimMontage(RunningTurnLeft, 1.6f);
					}
					else if (CharacterState == ECharacterState::ECS_Flying)
					{
						FlyingTurnRotation({ FlyingTurnRotationValue });
					}

					MoveLeftTimeline.PlayFromStart();
					MoveLeftTimeline.SetTimelineFinishedFunc(TimelineFinished);
				}
			}
		}
	}
}

void ASpaceRunnerCharacter::MoveRight()
{
	if (LevelManager->GetIsPlaying())
	{
		if (CharacterState == ECharacterState::ECS_Flying || CharacterState == ECharacterState::ECS_Running)
		{
			if (!bMoving)
			{
				if (CurrentLane != NumberOfLanes)
				{
					bIsTurningRight = true;
					bMoving = true;
					TargetPosition = GetActorLocation().Y + LaneWidth;
					FlyingTurnRotationValue = { 0.f,0.f, 30.f };

					if (CharacterState == ECharacterState::ECS_Running)
					{
						PlayAnimMontage(RunningTurnRight, 1.6f);
					}
					else if (CharacterState == ECharacterState::ECS_Flying)
					{
						FlyingTurnRotation({ FlyingTurnRotationValue });
					}

					MoveLeftTimeline.PlayFromStart();
					MoveLeftTimeline.SetTimelineFinishedFunc(TimelineFinished);
				}
			}
		}
	}
}

// Called every frame
void ASpaceRunnerCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (CharacterState == ECharacterState::ECS_Flying)
	{
		SetActorLocation({ GetActorLocation().X,GetActorLocation().Y, FlyingHeightZ });
	}
	else if (!GetCharacterMovement()->IsFalling() && CharacterState != ECharacterState::ECS_Sliding)
	{
		CharacterState = ECharacterState::ECS_Running;
	}

	MoveLeftTimeline.TickTimeline(DeltaTime);
}

// Called to bind functionality to input
void ASpaceRunnerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ASpaceRunnerCharacter::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);
	PlayerInputComponent->BindAction("Left", IE_Pressed, this, &ASpaceRunnerCharacter::MoveLeft);
	PlayerInputComponent->BindAction("Right", IE_Pressed, this, &ASpaceRunnerCharacter::MoveRight);
	PlayerInputComponent->BindAction("Down", IE_Pressed, this, &ASpaceRunnerCharacter::MoveDown);
	PlayerInputComponent->BindAction("Down", IE_Released, this, &ASpaceRunnerCharacter::StopSliding);
	PlayerInputComponent->BindAction("Any Key", IE_Pressed, this, &ASpaceRunnerCharacter::StartGame);
	PlayerInputComponent->BindAction("ESC", IE_Pressed, this, &ASpaceRunnerCharacter::ESCDown).bExecuteWhenPaused = true;
}

void ASpaceRunnerCharacter::SetupTimeline()
{
	if (MoveLeftFloat)
	{
		FOnTimelineFloat MoveLeftTimelineProgress;
		MoveLeftTimelineProgress.BindUFunction(this, FName("MoveLeftTimelineProgress"));
		MoveLeftTimeline.AddInterpFloat(MoveLeftFloat, MoveLeftTimelineProgress);
		TimelineFinished.BindUFunction(this, FName("TimelineFinishedFunction"));
	}
}

void ASpaceRunnerCharacter::MoveLeftTimelineProgress(float Value)
{
	float PlayerLocationY = FMath::Lerp(GetActorLocation().Y, TargetPosition, Value);

	FVector PlayerLocation = { GetActorLocation().X, PlayerLocationY, GetActorLocation().Z };

	SetActorLocation(PlayerLocation);
}

void ASpaceRunnerCharacter::TimelineFinishedFunction()
{
	bMoving = false;

	if (bIsTurningLeft)
	{
		CurrentLane--;
		bIsTurningLeft = false;
	}

	if (bIsTurningRight)
	{
		CurrentLane++;
		bIsTurningRight = false;
	}	
}

void ASpaceRunnerCharacter::Jump()
{
	if (CharacterState == ECharacterState::ECS_Running && LevelManager->GetIsPlaying())
	{
		ACharacter::Jump();
		CharacterState = ECharacterState::ECS_Jumping;
	}	
}

void ASpaceRunnerCharacter::StartFlying()
{
	CharacterState = ECharacterState::ECS_Flying;
	
	if (LevelManager && JetpackParticles && SpeedLines && JetpackMesh)
	{
		SetActorLocation({ GetActorLocation().X,GetActorLocation().Y, FlyingHeightZ });
		GetCharacterMovement()->GravityScale = 0.f;
		LevelManager->SetLevelSpeed(FlyingSpeed);
		JetpackParticles->SetVisibility(true);
		JetpackMesh->SetVisibility(true);
		SpeedLines->SetVisibility(true);
		//JetpackSFX->Play();

		TArray<AActor*> AllCoins;
		UGameplayStatics::GetAllActorsOfClass(GetWorld(), ACoin::StaticClass(), AllCoins);

		for (AActor* coin : AllCoins)
		{
			ACoin* myCoin = Cast<ACoin>(coin);

			if (myCoin != nullptr)
			{
				myCoin->MoveCoinsUp();
			}
		}
	}	
}

void ASpaceRunnerCharacter::StopFlying()
{
	CharacterState = ECharacterState::ECS_Falling;

	if (LevelManager && JetpackParticles && SpeedLines && JetpackMesh)
	{
		LevelManager->SetLevelSpeed(LevelManager->GetCurrentLevelSpeed());
		GetCharacterMovement()->GravityScale = 0.7f;
		JetpackParticles->SetVisibility(false);
		JetpackMesh->SetVisibility(false);
		SpeedLines->SetVisibility(false);
		//JetpackSFX->Stop();

		TArray<AActor*> AllCoins;
		UGameplayStatics::GetAllActorsOfClass(GetWorld(), ACoin::StaticClass(), AllCoins);

		for (AActor* coin : AllCoins)
		{
			ACoin* myCoin = Cast<ACoin>(coin);

			if (myCoin != nullptr)
			{
				myCoin->MoveCoinsDown();
			}
		}
	}
}

void ASpaceRunnerCharacter::MoveDown()
{
	UE_LOG(LogTemp, Warning, TEXT("Move down pressed"));
	if (CharacterState != ECharacterState::ECS_Flying)
	{
		if (!LevelManager->GetIsPlaying() || GetCharacterMovement()->IsFalling())
		{
			UE_LOG(LogTemp, Warning, TEXT("Add impulse?"));
			GetCharacterMovement()->AddImpulse({ 0.f,0.f,-3000.f }, true);
		}

		else if (CharacterState == ECharacterState::ECS_Running)
		{
			Slide();
		}	
	}
}

void ASpaceRunnerCharacter::Slide()
{
	CharacterState = ECharacterState::ECS_Sliding;
	Crouch();
}

void ASpaceRunnerCharacter::StopSliding()
{
	if (CharacterState == ECharacterState::ECS_Sliding)
	{
		CharacterState = ECharacterState::ECS_Running;
		UnCrouch();
	}	
}

void ASpaceRunnerCharacter::ESCDown()
{
	if (RunnerPlayerController && LevelManager->GetIsPlaying())
	{
		RunnerPlayerController->TogglePauseMenu();
	}
}

void ASpaceRunnerCharacter::MagnetOn()
{
	if (MagnetSphereCollision)
	{
		MagnetSphereCollision->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	}	
}

void ASpaceRunnerCharacter::MagnetOff()
{
	if (MagnetSphereCollision)
	{
		MagnetSphereCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}
}

void ASpaceRunnerCharacter::ForceFieldOn()
{
	if (ForceFieldFX && ForceFieldSphereCollision)
	{
		ForceFieldFX->SetVisibility(true);
		ForceFieldSphereCollision->SetCollisionProfileName(TEXT("Forcefield"));
		bForceFieldOn = true;
	}
	
}

void ASpaceRunnerCharacter::ForceFieldOff()
{
	if (ForceFieldFX && ForceFieldSphereCollision)
	{
		ForceFieldFX->SetVisibility(false);
		ForceFieldSphereCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		bForceFieldOn = false;
	}
}

void ASpaceRunnerCharacter::RestartLevel()
{

}

void ASpaceRunnerCharacter::OnOverlapBeginMagnet(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	AActor* Coin = Cast<ACoin>(OtherActor);

	if (Coin && Coin != nullptr)
	{
		Cast<ACoin>(OtherActor)->PickupCoin();
	}
}

void ASpaceRunnerCharacter::OnOverlapBeginForceField(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	AActor* Obstacle = Cast<AObstacle>(OtherActor);

	if (Obstacle)
	{
		if (bForceFieldOn)
		{
			//Obstacle->GetMesh();
			Cast<AObstacle>(OtherActor)->Dissolve();
		}
		
	}
}

void ASpaceRunnerCharacter::StartGame()
{
	LevelManager->StartGame();
}

void ASpaceRunnerCharacter::DecreaseOxygen()
{
	Oxygen -= 0.01f;

	if (Oxygen <= 0.f)
	{
		CharacterState = ECharacterState::ECS_Dead;
		LevelManager->GameOver_Implementation();
		GetWorldTimerManager().ClearTimer(DecreaseOxygenTimerHandle);
	}
}

void ASpaceRunnerCharacter::RestoreOxygen()
{
	GetWorldTimerManager().ClearTimer(DecreaseOxygenTimerHandle);

	GetWorldTimerManager().SetTimer(RestoreOxygenTimerHandle, this, &ASpaceRunnerCharacter::RestoreOxygen, 0.01f, true, 0.f);

	Oxygen += FMath::Clamp(RestoreOxygenAmount, 0.f, MaxOxygen);

	if (OxygenRestoreFX)
	{
		OxygenRestoreFX->SetVisibility(true);
	}	
}

void ASpaceRunnerCharacter::StopOxygen()
{
	GetWorldTimerManager().ClearTimer(RestoreOxygenTimerHandle);

	GetWorldTimerManager().SetTimer(DecreaseOxygenTimerHandle, this, &ASpaceRunnerCharacter::DecreaseOxygen, 0.01f, true, 1.f);

	if (OxygenRestoreFX)
	{
		OxygenRestoreFX->SetVisibility(false);
	}	
}

void ASpaceRunnerCharacter::QuizPickup()
{
	/*
	if (QuizWidget)
	{
		QuizWidget = CreateDefaultSubobject<UUserWidget>(TEXT("QuizWidget"));
		QuizWidget->AddToViewport();
		bQuizIsActive = true;
	}
	
	GetWorldTimerManager().SetTimer(QuizTimerHandle, this, &ASpaceRunnerCharacter::CheckPlayerLane, 0.01f, false, 3.0f);
	*/
}

void ASpaceRunnerCharacter::CheckPlayerAnswer()
{
	switch (CurrentLane)
	{
	case 1:
		// Set player answer to true
		break;
	case 2:
		break;
	case 3:
		// Set player answer to false
		break;
	}

	if (true)
	{
		Coins += 100;
		CoinsTotal += 100;
		// Play Correct sound
		// Set Text to Correct!
	}
	else
	{
		// Play Incorrect sound
		// Set Text to Incorrect!
	}

	//QuizWidget->RemoveFromParent();

	// Create HUD C++ class to access WBP Timer and call StopTimer()
	bQuizIsActive = false;
}

