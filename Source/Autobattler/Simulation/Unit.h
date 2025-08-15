#pragma once
#include <string>

#include "HexCoordinate.h"

struct HexCoordinate;

enum class ETeamColor
{
	Red,
	Blue
};

enum class EUnitState
{
	Idle,
	Moving,
	Attacking,
	Dead
};


class Unit
{
public:
	Unit(const int ID, const HexCoordinate& Pos, const ETeamColor Team, const int HP, const int AttackCooldown = 3);

private:
	int ID;
	int HP;
	int MaxHP;
	int AttackCooldown;
	int MaxAttackCooldown;
	int TargetUnitID;
	std::string Name;
	HexCoordinate Position;
	ETeamColor Team;
	EUnitState State;
};
