#include "BattleSimulation.h"

BattleSimulation::BattleSimulation(const SimulationConfig& Config) :
    Grid(Config.GridRadius),
    RNG(Config.RandomSeed),
    CurrentStep(0),
    Config(Config),
    bIsFinished(false),
    CurrentSimTime(0),
    Winner(ETeamColor::None),
    NextUnitId(0)
{
    OccupiedPositions.reserve(Config.UnitsPerTeam * 2);
    UnitTargetPairs.reserve(Config.UnitsPerTeam * 2);
    Units.reserve(Config.UnitsPerTeam * 2);
}

void BattleSimulation::Initialize()
{
    CurrentStep = 0;
    CurrentSimTime = 0.0f;
    NextUnitId = 0;
    Units.clear();
    RedTeamIds.clear();
    BlueTeamIds.clear();
    PositionToUnitId.clear();
    OccupiedPositions.clear();
    UnitTargetPairs.clear();
    bIsFinished = false;
    RNG.Seed(Config.RandomSeed);
        
    // Create red team
    for (int i = 0; i < Config.UnitsPerTeam; i++)
    {
        HexCoordinate Pos = FindEmptyPosition();
        int HP = RNG.Range(Config.MinHP, Config.MaxHP);
        auto NewUnit = std::make_unique<Unit>(GenerateUnitId(), Pos, ETeamColor::Red, HP, Config.AttackCooldownSteps);
        AddUnit(std::move(NewUnit));
    }
        
    // Create blue team
    for (int i = 0; i < Config.UnitsPerTeam; i++)
    {
        HexCoordinate Pos = FindEmptyPosition();
        int HP = RNG.Range(Config.MinHP, Config.MaxHP);
        auto NewUnit = std::make_unique<Unit>(GenerateUnitId(), Pos, ETeamColor::Blue, HP, Config.AttackCooldownSteps);
        AddUnit(std::move(NewUnit));
    }
}

void BattleSimulation::StepSimulation()
{
	if (bIsFinished) return;
        
    CurrentStep++;
    CurrentSimTime += Config.TimeStepDuration;
    
    OccupiedPositions.clear();
    Grid.InvalidatePathCache();
    UnitTargetPairs.clear();
    for (auto& [ID, Unit] : Units)
    {
        Unit->UpdateCooldowns();
        if (Unit->IsAlive())
        {
            OccupiedPositions.insert(Unit->GetPosition());
            if (int TargetId = FindClosestEnemyId(*Unit); TargetId != -1)
            {
                UnitTargetPairs.push_back({ID, TargetId});
            }
        }
    }
    
    Grid.InvalidatePathCache();
    
    for (const auto& [unitId, targetId] : UnitTargetPairs)
    {
        Unit* unit = GetUnitById(unitId);
        
        Unit* target = GetUnitById(targetId);
        
        if (!unit || !unit->IsAlive() || !target) continue;
        
        int Distance = unit->GetPosition().DistanceTo(target->GetPosition());
        
        if (Distance <= Config.AttackRange)
        {
            unit->TryAttack(*target);
        }
        else
        {
            HexCoordinate oldPos = unit->GetPosition();
            OccupiedPositions.erase(oldPos);
            
            unit->MoveTowardAttackRange(target->GetPosition(), Config.AttackRange, Grid, OccupiedPositions);
            
            HexCoordinate newPos = unit->GetPosition();
            if (oldPos != newPos)
            {
                UpdateUnitPosition(unit, oldPos, newPos);
                OccupiedPositions.insert(newPos);
            }
            else
            {
                OccupiedPositions.insert(oldPos);
            }
        }
    }
    
    RemoveDeadUnits();
    CheckWinCondition();
}

int BattleSimulation::FindClosestEnemyId(const Unit& Unit)
{
    int ClosestId = -1;
    int MinDistance = std::numeric_limits<int>::max();
    
    // Only check enemy team units

    for (const auto& EnemyIds = (Unit.GetTeam() == ETeamColor::Red) ? BlueTeamIds : RedTeamIds; const int EnemyId : EnemyIds)
    {
        const auto* Enemy = GetUnitById(EnemyId);
        if (!Enemy || !Enemy->IsAlive()) continue;

        if (const int Distance = Unit.GetPosition().DistanceTo(Enemy->GetPosition()); Distance < MinDistance)
        {
            MinDistance = Distance;
            ClosestId = EnemyId;
        }
    }
    
    return ClosestId;
}

Unit* BattleSimulation::GetUnitById(const int ID)
{
    const auto It = Units.find(ID);
    return (It != Units.end()) ? It->second.get() : nullptr;
}

void BattleSimulation::UpdateUnitPosition(Unit* Unit, const HexCoordinate& OldPos, const HexCoordinate& NewPos)
{
    PositionToUnitId.erase(OldPos);
    PositionToUnitId[NewPos] = Unit->GetId();
    Unit->SetPosition(NewPos);
}

void BattleSimulation::RemoveDeadUnits()
{
    std::vector<int> ToRemove;
    for (const auto& [id, unit] : Units)
    {
        if (!unit->IsAlive())
        {
            ToRemove.push_back(id);
        }
    }

    for (const int ID : ToRemove)
    {
        RemoveUnit(ID);
    }
}

void BattleSimulation::RemoveUnit(const int ID)
{
    if (const auto It = Units.find(ID); It != Units.end())
    {
        PositionToUnitId.erase(It->second->GetPosition());
        
        RedTeamIds.erase(ID);
        BlueTeamIds.erase(ID);
        
        Units.erase(It);
    }
}

void BattleSimulation::CheckWinCondition()
{
    int RedAlive = 0;
    int BlueAlive = 0;
    
    for (const int ID : RedTeamIds)
    {
        if (GetUnitById(ID)->IsAlive()) RedAlive++;
    }
    
    for (const int ID : BlueTeamIds)
    {
        if (GetUnitById(ID)->IsAlive()) BlueAlive++;
    }
    
    if (RedAlive == 0 && BlueAlive == 0)
    {
        bIsFinished = true;
    }
    else if (RedAlive == 0) 
    {
        bIsFinished = true;
        Winner = ETeamColor::Blue;
    }
    else if (BlueAlive == 0)
    {
        bIsFinished = true;
        Winner = ETeamColor::Red;
    }
}

void BattleSimulation::AddUnit(std::unique_ptr<Unit> Unit)
{
    const int ID = Unit->GetId();
    const HexCoordinate Position = Unit->GetPosition();
    const ETeamColor Team = Unit->GetTeam();
    
    PositionToUnitId[Position] = ID;
    
    if (Team == ETeamColor::Red)
    {
        RedTeamIds.insert(ID);
    }
    else
    {
        BlueTeamIds.insert(ID);
    }
    
    Units[ID] = std::move(Unit);
}

HexCoordinate BattleSimulation::FindEmptyPosition()
{
    HexCoordinate Pos;
    int Attempts = 0;
    do {
        Pos = RNG.RandomHexCoordinate(Grid.GetRadius());
        Attempts++;
        if (Attempts > 1000)
        {
            // Fallback: find first empty position
            for (const auto AllCoords = Grid.GetAllValidCoords(); const auto& Cord : AllCoords)
            {
                if (!PositionToUnitId.contains(Cord))
                {
                    return Cord;
                }
            }
            return HexCoordinate(0, 0);
        }
    } while (PositionToUnitId.contains(Pos));
        
    return Pos;
}

int BattleSimulation::GenerateUnitId()
{
    return NextUnitId++;
}
