// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "HexBattleGameMode.generated.h"

/**
 * 
 */
UCLASS()
class AUTOBATTLER_API AHexBattleGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	AHexBattleGameMode();
	virtual void BeginPlay() override;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Simulation")
	TSubclassOf<class ASimulationManager> SimulationManagerClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera")
	float CameraHeight = 4000.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera")
	float CameraAngle = -60.0f;

private:
	UPROPERTY()
	class ASimulationManager* SimulationManager;

	void SetupCamera();
	void SpawnSimulationManager();
	
};
