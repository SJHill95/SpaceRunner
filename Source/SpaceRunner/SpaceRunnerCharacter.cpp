// Fill out your copyright notice in the Description page of Project Settings.


#include "SpaceRunnerCharacter.h"
#include "Components/CapsuleComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "LevelManager.h"
#include "EngineUtils.h"
#include "Kismet/GameplayStatics.h"
#include "Components/WidgetComponent.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Coin.h"
#include "NiagaraComponent.h"

// Sets default values
ASpaceRunnerCharacter::ASpaceRunnerCharacter() :
	// Jumping variables
	JumpVelocity(600.f),
	GravityScale(1.f),
	// Movement
	CharacterSpeed(600.f),
	bIsRunnerCharacter(true),
	// Coins
	Coins(0.f),
	CoinsTotal(0.f),
	// Flying
	FlyingHeightZ(400.f),
	bIsFlying(false),
	FlyingSpeed(-2000.f)
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

	tempPos = GetActorLocation();
	zPosition = tempPos.Z + 300.f;

	// Magnet Collision
	MagnetSphereCollision = CreateDefaultSubobject<USphereComponent>(TEXT("MagnetSphereCollision"));
	MagnetSphereCollision->SetupAttachment(RootComponent);

	// Jetpack/Flying Particles and mesh
	JetpackMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("JetpackMesh"));
	JetpackMesh->SetupAttachment(GetMesh(), GetMesh()->GetSocketBoneName(TEXT("spine_03")));

	JetpackParticles = CreateDefaultSubobject<UNiagaraComponent>(TEXT("JetpackParticles"));
	JetpackParticles->SetupAttachment(JetpackMesh);

	SpeedLines = CreateDefaultSubobject<UNiagaraComponent>(TEXT("SpeedLines"));
	SpeedLines->SetupAttachment(RootComponent);

}

// Called when the game starts or when spawned
void ASpaceRunnerCharacter::BeginPlay()
{
	Super::BeginPlay();

	CanMove = true;

	LevelManager = Cast<ALevelManager>(UGameplayStatics::GetActorOfClass(GetWorld(), ALevelManager::StaticClass()));

	Initialize();

	if (bIsRunnerCharacter)
	{
		CharacterType = ECharacterType::ECT_Runner;
	}
	else
	{
		CharacterType = ECharacterType::ECT_Freeroam;
	}

	MagnetSphereCollision->OnComponentBeginOverlap.AddDynamic(this, &ASpaceRunnerCharacter::OnOverlapBegin);

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

void ASpaceRunnerCharacter::MoveLeft_Implementation()
{
	
}

void ASpaceRunnerCharacter::MoveRight_Implementation()
{

}

// Called every frame
void ASpaceRunnerCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	tempPos = GetActorLocation();
	tempPos.X -= 850.f;
	tempPos.Z = zPosition;

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
	//PlayerInputComponent->BindAxis("MoveRight", this, &ASpaceRunnerCharacter::MoveRight);
}

void ASpaceRunnerCharacter::MoveRight(float value)
{
	if (CanMove)
	{
		const FVector Direction{ 0.f, 1.f, 0.f };
		AddMovementInput(Direction, value);
	}
}

void ASpaceRunnerCharacter::Jump()
{
	if (CharacterType == ECharacterType::ECT_Freeroam)
	{
		ACharacter::Jump();
	}

	else if (CharacterType == ECharacterType::ECT_Runner)
	{
		if (!bIsFlying && LevelManager->GetIsPlaying())
		{
			ACharacter::Jump();
		}
	}
	
}

void ASpaceRunnerCharacter::StartFlying()
{
	CharacterState = ECharacterState::ECS_Flying;

	SetActorLocation({ GetActorLocation().X,GetActorLocation().Y, FlyingHeightZ });
	GetCharacterMovement()->GravityScale = 0.f;
	LevelManager->SetLevelSpeed(FlyingSpeed);
}

void ASpaceRunnerCharacter::StopFlying()
{
	CharacterState = ECharacterState::ECS_Falling;
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

		else
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

void ASpaceRunnerCharacter::RestartLevel()
{

}

void ASpaceRunnerCharacter::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	AActor* Coin = Cast<ACoin>(OtherActor);

	if (Coin && Coin != nullptr)
	{
		Cast<ACoin>(OtherActor)->PickupCoin();
	}
}

void ASpaceRunnerCharacter::StartGame()
{
	LevelManager->StartGame();
}

