#pragma once
#include "HexGrid.h"
#include "RDSimulation.h"
#include "Unit.h"

enum class ETeamColor;

struct SimulationConfig
{
	int GridRadius = 8;
	int UnitsPerTeam = 3;
	int MoveSpeedPerStep = 1;
	int AttackRange = 2;
	int AttackCooldownSteps = 3;
	int MinHP = 2;
	int MaxHP = 5;
	uint32_t RandomSeed = 12345;
	float TimeStepDuration = 0.1f;
};

struct SimulationState
{
	uint32_t CurrentStep;
	float CurrentSimTime;
	bool bIsFinished;
	ETeamColor Winner;
};

class BattleSimulation
{
public:
	BattleSimulation(const SimulationConfig& Config = SimulationConfig());

	void Initialize();
	void StepSimulation();
	int FindClosestEnemyId(const Unit& Unit);
	Unit* GetUnitById(int ID);
	void UpdateUnitPosition(Unit* Unit, const HexCoordinate& OldPos, const HexCoordinate& NewPos);
	void RemoveDeadUnits();
	void RemoveUnit(int ID);
	void CheckWinCondition();
	void AddUnit(std::unique_ptr<Unit> Unit);

	bool IsFinished() const { return bIsFinished; }
	ETeamColor GetWinner() const { return Winner; }
	uint32_t GetCurrentStep() const { return CurrentStep; }
	float GetCurrentSimTime() const { return CurrentSimTime; }

	HexCoordinate FindEmptyPosition();
	int GenerateUnitId();
private:
	HexGrid Grid;
	std::unordered_map<int, std::unique_ptr<Unit>> Units;
	std::unordered_set<HexCoordinate> OccupiedPositions;
	std::vector<std::pair<int, int>> UnitTargetPairs;
	std::unordered_set<int> RedTeamIds; 
	std::unordered_set<int> BlueTeamIds;
	std::unordered_map<HexCoordinate, int> PositionToUnitId;
	
	RDSimulation RNG;
	uint32_t CurrentStep;
	SimulationConfig Config;
	bool bIsFinished;
	float CurrentSimTime;
	ETeamColor Winner;
	int NextUnitId;
};
