// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Autobattler/Simulation/BattleSimulation.h"
#include "Autobattler/Simulation/Unit.h"
#include "GameFramework/Actor.h"
#include "SimulationManager.generated.h"

class BattleSimulation;
class Unit;
struct HexCoordinate;

USTRUCT(BlueprintType)
struct FUnitVisualData
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly)
	int32 UnitId = -1;

	UPROPERTY(BlueprintReadOnly)
	FVector Position = FVector::ZeroVector;

	UPROPERTY(BlueprintReadOnly)
	uint8 Team = static_cast<int>(ETeamColor::Red);

	UPROPERTY(BlueprintReadOnly)
	int32 CurrentHP = 0;

	UPROPERTY(BlueprintReadOnly)
	int32 MaxHP = 0;

	UPROPERTY(BlueprintReadOnly)
	bool bIsAttacking = false;

	UPROPERTY(BlueprintReadOnly)
	bool bIsAlive = true;
};

UCLASS()
class AUTOBATTLER_API ASimulationManager : public AActor
{
	GENERATED_BODY()
	
public:
    ASimulationManager();
    virtual void BeginPlay() override;
    virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
    virtual void Tick(float DeltaTime) override;
	
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Simulation Config")
    int32 GridRadius = 10;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Simulation Config")
    int32 UnitsPerTeam = 3;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Simulation Config")
    int32 AttackRange = 2;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Simulation Config")
    int32 RandomSeed = 12345;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Simulation Config")
    float SimulationTimeStep = 0.1f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Simulation Config")
    bool bAutoRun = true;
	
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Visualization")
    TSubclassOf<class AHexTile> HexTileClass;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Visualization")
    TSubclassOf<class AUnitVisualizer> UnitVisualizerClass;
	
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Visualization")
    float HexSize = 100.0f;
	
    UPROPERTY(BlueprintReadOnly, Category = "Simulation State")
    int32 CurrentStep = 0;

    UPROPERTY(BlueprintReadOnly, Category = "Simulation State")
    float CurrentSimTime = 0.0f;

    UPROPERTY(BlueprintReadOnly, Category = "Simulation State")
    bool bIsSimulationFinished = false;
	
    UFUNCTION(BlueprintCallable, Category = "Simulation")
    void InitializeSimulation();

    UFUNCTION(BlueprintCallable, Category = "Simulation")
    void StepSimulation();

    UFUNCTION(BlueprintCallable, Category = "Simulation")
    TArray<FUnitVisualData> GetAllUnitsData() const;

    UFUNCTION(BlueprintPure, Category = "Hex Conversion")
    FVector HexToWorldPosition(int32 Q, int32 R) const;
	
    UFUNCTION(BlueprintImplementableEvent, Category = "Simulation Events")
    void OnSimulationStarted();

    UFUNCTION(BlueprintImplementableEvent, Category = "Simulation Events")
    void OnSimulationStep();

    UFUNCTION(BlueprintImplementableEvent, Category = "Simulation Events")
    void OnSimulationFinished();

private:
    TUniquePtr<BattleSimulation> Simulation;
    TUniquePtr<SimulationConfig> Config;
    float TimeAccumulator = 0.0f;

    UPROPERTY()
    TArray<class AHexTile*> HexTiles;

    UPROPERTY()
    TMap<int32, class AUnitVisualizer*> UnitVisualizers;

    void CreateHexGrid();
    void CreateUnitVisualizers();
    void UpdateUnitVisualizers();
    void CleanupVisualizers();
    FVector ConvertHexToWorld(const HexCoordinate& Hex) const;

};
