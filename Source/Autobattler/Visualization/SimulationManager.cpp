// Fill out your copyright notice in the Description page of Project Settings.
#include "SimulationManager.h"

#include "HexTile.h"
#include "UnitVisualizer.h"
#include "../Simulation/BattleSimulation.h"
#include "../Simulation/Unit.h"
#include "../Simulation/HexCoordinate.h"

// Sets default values
ASimulationManager::ASimulationManager()
{
    PrimaryActorTick.bCanEverTick = true;
    RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));
}

void ASimulationManager::BeginPlay()
{
    Super::BeginPlay();
    
    if (bAutoRun)
    {
        InitializeSimulation();
    }
}

void ASimulationManager::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
    CleanupVisualizers();
    
    Super::EndPlay(EndPlayReason);
}

void ASimulationManager::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    if (!Simulation || bIsSimulationFinished || !bAutoRun)
    {
        return;
    }

    // Fixed timestep simulation
    TimeAccumulator += DeltaTime;
    
    while (TimeAccumulator >= SimulationTimeStep)
    {
        StepSimulation();
        TimeAccumulator -= SimulationTimeStep;
        
        if (bIsSimulationFinished)
        {
            break;
        }
    }

    // Update visual interpolation every frame
    UpdateUnitVisualizers();
}

void ASimulationManager::InitializeSimulation()
{
    CleanupVisualizers();
    
    Config = MakeUnique<SimulationConfig>();
    Config->GridRadius = GridRadius;
    Config->UnitsPerTeam = UnitsPerTeam;
    Config->AttackRange = AttackRange;
    Config->RandomSeed = RandomSeed;
    Config->TimeStepDuration = SimulationTimeStep;
    
    Simulation = MakeUnique<BattleSimulation>(*Config);
    Simulation->Initialize();
    
    CurrentStep = 0;
    CurrentSimTime = 0.0f;
    bIsSimulationFinished = false;
    TimeAccumulator = 0.0f;
    
    CreateHexGrid();
    CreateUnitVisualizers();

    OnSimulationStarted();
    UE_LOG(LogTemp, Warning, TEXT("Simulation initialized with seed: %d"), RandomSeed);
}

void ASimulationManager::StepSimulation()
{
    if (!Simulation || bIsSimulationFinished)
    {
        return;
    }

    Simulation->StepSimulation();

    auto State = Simulation->GetCurrentState();
    CurrentStep = State.CurrentStep;
    CurrentSimTime = State.CurrentSimTime;
    bIsSimulationFinished = State.bIsFinished;

    if (bIsSimulationFinished)
    {
        OnSimulationFinished();
    }

    UpdateUnitVisualizers();
    OnSimulationStep();
}

TArray<FUnitVisualData> ASimulationManager::GetAllUnitsData() const
{
    TArray<FUnitVisualData> Result;
    
    if (!Simulation)
    {
        return Result;
    }

    const auto& Units = Simulation->GetUnits();
    for (const auto& [Id, Unit] : Units)
    {
        if (!Unit) continue;

        FUnitVisualData Data;
        Data.UnitId = Unit->GetId();
        Data.Position = ConvertHexToWorld(Unit->GetPosition());
        Data.Team = static_cast<uint8>((Unit->GetTeam() == ETeamColor::Red) ? ETeamColor::Red : ETeamColor::Blue);
        Data.CurrentHP = Unit->GetHP();
        Data.MaxHP = Unit->GetMaxHP();
        Data.bIsAttacking = (Unit->GetState() == EUnitState::Attacking);
        Data.bIsAlive = Unit->IsAlive();
        
        Result.Add(Data);
    }

    return Result;
}

FVector ASimulationManager::HexToWorldPosition(int32 Q, int32 R) const
{
    HexCoordinate Hex(Q, R);
    return ConvertHexToWorld(Hex);
}

FVector ASimulationManager::ConvertHexToWorld(const HexCoordinate& Hex) const
{
    // Axial to world position conversion for pointy-top hexagons
    float X = HexSize * (FMath::Sqrt(3.0f) * Hex.q + FMath::Sqrt(3.0f) / 2.0f * Hex.r);
    float Y = HexSize * (3.0f / 2.0f * Hex.r);
    return FVector(X, Y, 0.0f);
}

void ASimulationManager::CreateHexGrid()
{
    if (!HexTileClass || !Simulation)
    {
        UE_LOG(LogTemp, Warning, TEXT("HexTileClass not set or Simulation not initialized"));
        return;
    }
    
    for (AHexTile* Tile : HexTiles)
    {
        if (Tile) Tile->Destroy();
    }
    HexTiles.Empty();
    
    const auto& Grid = Simulation->GetGrid();
    auto ValidCoords = Grid.GetAllValidCoords();
    
    for (const auto& Coord : ValidCoords)
    {
        FVector WorldPos = ConvertHexToWorld(Coord) + GetActorLocation();
        FTransform SpawnTransform(FRotator::ZeroRotator, WorldPos);

        if (AHexTile* NewTile = GetWorld()->SpawnActor<AHexTile>(
            HexTileClass, SpawnTransform))
        {
            NewTile->SetCoordinates(Coord.q, Coord.r);
            NewTile->AttachToActor(this, FAttachmentTransformRules::KeepWorldTransform);
            HexTiles.Add(NewTile);
        }
    }
}

void ASimulationManager::CreateUnitVisualizers()
{
    if (!UnitVisualizerClass || !Simulation)
    {
        return;
    }
    
    const auto& Units = Simulation->GetUnits();
    
    for (const auto& [Id, Unit] : Units)
    {
        if (!Unit) continue;
    
        FVector WorldPos = ConvertHexToWorld(Unit->GetPosition()) + GetActorLocation();
        WorldPos.Z += 50.0f; // Raise units above the grid
        
        FTransform SpawnTransform(FRotator::ZeroRotator, WorldPos);
        
        AUnitVisualizer* NewVisualizer = GetWorld()->SpawnActor<AUnitVisualizer>(
            UnitVisualizerClass, SpawnTransform);
        
        if (NewVisualizer)
        {
            NewVisualizer->Initialize(
                Unit->GetId(),
                static_cast<uint8>((Unit->GetTeam() == ETeamColor::Red) ? ETeamColor::Red : ETeamColor::Blue),
                Unit->GetHP(),
                Unit->GetMaxHP()
            );
            
            NewVisualizer->AttachToActor(this, FAttachmentTransformRules::KeepWorldTransform);
            UnitVisualizers.Add(Id, NewVisualizer);
        }
    }
}

void ASimulationManager::UpdateUnitVisualizers()
{
    if (!Simulation) return;
    
    const auto& Units = Simulation->GetUnits();
    
    TArray<int32> ToRemove;
    
    for (auto& [Id, Visualizer] : UnitVisualizers)
    {
        if (!Visualizer) continue;
    
        auto It = Units.find(Id);
        if (It != Units.end() && It->second && It->second->IsAlive())
        {
            FVector TargetPos = ConvertHexToWorld(It->second->GetPosition()) + GetActorLocation();
            TargetPos.Z += 50.0f;
            
            Visualizer->SetTargetPosition(TargetPos);
            Visualizer->SetHP(It->second->GetHP());
            Visualizer->SetAttacking(It->second->GetState() == EUnitState::Attacking);
        }
        else
        {
            Visualizer->SetAlive(false);
            ToRemove.Add(Id);
        }
    }
    
    for (int32 Id : ToRemove)
    {
        if (AUnitVisualizer* Visualizer = UnitVisualizers[Id])
        {
            Visualizer->PlayDeathEffect();
            UnitVisualizers.Remove(Id);
        }
    }
}

void ASimulationManager::CleanupVisualizers()
{
    for (AHexTile* Tile : HexTiles)
    {
        if (Tile) Tile->Destroy();
    }
    HexTiles.Empty();
    
    for (auto& [Id, Visualizer] : UnitVisualizers)
    {
        if (Visualizer) Visualizer->Destroy();
    }
    UnitVisualizers.Empty();
}

