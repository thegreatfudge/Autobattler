#include "BattleSimulation.h"

BattleSimulation::BattleSimulation(const SimulationConfig& Config) :
	Grid(Config.GridRadius),
	RNG(Config.RandomSeed),
	CurrentStep(0),
	Config(Config),
	bIsFinished(false)
{
	Units.reserve(Config.UnitsPerTeam * 2);
}
