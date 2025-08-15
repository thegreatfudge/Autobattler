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
	
private:
	HexGrid Grid;
	std::unordered_map<int, std::unique_ptr<Unit>> Units;
	
	RDSimulation RNG;
	uint32_t CurrentStep;
	SimulationConfig Config;
	bool bIsFinished;
};
