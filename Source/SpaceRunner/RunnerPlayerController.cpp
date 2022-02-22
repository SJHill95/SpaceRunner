// Fill out your copyright notice in the Description page of Project Settings.


#include "RunnerPlayerController.h"
#include "Blueprint/UserWidget.h"
#include "Kismet/GameplayStatics.h"


// Called when the game starts or when spawned
void ARunnerPlayerController::BeginPlay()
{
	Super::BeginPlay();

	/*
	if (WPauseMenu)
	{
		PauseMenu = CreateWidget<UUserWidget>(this, WPauseMenu);

		if (PauseMenu)
		{
			PauseMenu->AddToViewport();
			PauseMenu->SetVisibility(ESlateVisibility::Hidden);
		}
	}
	*/
}

void ARunnerPlayerController::TogglePauseMenu()
{
	if (bPauseMenuVisible)
	{
		RemovePauseMenu();
	}
	else
	{
		DisplayPauseMenu();
	}
}

void ARunnerPlayerController::DisplayPauseMenu_Implementation()
{

	bPauseMenuVisible = true;
	//PauseMenu->SetVisibility(ESlateVisibility::Visible);
	UGameplayStatics::SetGamePaused(this, true);
	FInputModeGameAndUI InputModeGameAndUI;
	SetInputMode(InputModeGameAndUI);
	bShowMouseCursor = true;

	if (PauseMenu)
	{
		
	}
}

void ARunnerPlayerController::RemovePauseMenu_Implementation()
{
	bPauseMenuVisible = false;
	//PauseMenu->SetVisibility(ESlateVisibility::Hidden);
	UGameplayStatics::SetGamePaused(this, false);
	FInputModeGameOnly InputModeGameOnly;
	SetInputMode(InputModeGameOnly);
	bShowMouseCursor = false;

	if (PauseMenu)
	{
		
	}

}
