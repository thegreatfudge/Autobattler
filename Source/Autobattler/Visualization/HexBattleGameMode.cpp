// Fill out your copyright notice in the Description page of Project Settings.


#include "HexBattleGameMode.h"

#include "SimulationManager.h"
#include "Camera/CameraActor.h"
#include "GameFramework/SpectatorPawn.h"
#include "Kismet/GameplayStatics.h"

AHexBattleGameMode::AHexBattleGameMode(): SimulationManager(nullptr)
{
	DefaultPawnClass = ASpectatorPawn::StaticClass();
}

void AHexBattleGameMode::BeginPlay()
{
	Super::BeginPlay();
    
	SpawnSimulationManager();
	SetupCamera();
}

void AHexBattleGameMode::SpawnSimulationManager()
{
	if (!SimulationManagerClass)
	{
		UE_LOG(LogTemp, Error, TEXT("SimulationManagerClass not set in GameMode!"));
		return;
	}

	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	SimulationManager = GetWorld()->SpawnActor<ASimulationManager>(
		SimulationManagerClass, FVector::ZeroVector, FRotator::ZeroRotator, SpawnParams);
}

void AHexBattleGameMode::SetupCamera()
{
	FVector CameraLocation = FVector(-500.0f, 0.0f, CameraHeight);
	FRotator CameraRotation = FRotator(CameraAngle, 0.0f, 0.0f);
    
	ACameraActor* CameraActor = GetWorld()->SpawnActor<ACameraActor>(
		CameraLocation, CameraRotation);
    
	if (CameraActor)
	{
		APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0);
		if (PC)
		{
			PC->SetViewTarget(CameraActor);
			PC->bShowMouseCursor = true;
		}
	}
}