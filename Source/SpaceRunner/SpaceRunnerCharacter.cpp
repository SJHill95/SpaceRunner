// Fill out your copyright notice in the Description page of Project Settings.


#include "SpaceRunnerCharacter.h"
#include "Components/CapsuleComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "LevelManager.h"
#include "EngineUtils.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
ASpaceRunnerCharacter::ASpaceRunnerCharacter() :
	// Jumping variables
	JumpVelocity(600.f),
	GravityScale(1.f),
	// Movement
	CharacterSpeed(600.f),
	bIsFlying(false)
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


}

// Called when the game starts or when spawned
void ASpaceRunnerCharacter::BeginPlay()
{
	Super::BeginPlay();

	CanMove = true;

	Initialize();

}

void ASpaceRunnerCharacter::Initialize()
{
	LevelManager = Cast<ALevelManager>(UGameplayStatics::GetActorOfClass(GetWorld(), ALevelManager::StaticClass()));
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
	
	if (!bIsFlying && LevelManager->GetIsPlaying())
	{
		ACharacter::Jump();
	}
	
}

void ASpaceRunnerCharacter::RestartLevel()
{

}

void ASpaceRunnerCharacter::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{

}

