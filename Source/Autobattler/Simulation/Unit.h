#pragma once
#include <string>
#include <unordered_set>

#include "HexCoordinate.h"

class HexGrid;
struct HexCoordinate;

enum class ETeamColor
{
	None,
	
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

	int GetId() const { return ID; }
	const std::string& GetName() const { return Name; }
	const HexCoordinate& GetPosition() const { return Position; }
	ETeamColor GetTeam() const { return Team; }
	int GetHP() const { return HP; }
	int GetMaxHP() const { return MaxHP; }
	bool IsAlive() const { return HP > 0 && State != EUnitState::Dead; }
	EUnitState GetState() const { return State; }
	bool CanAttack() const { return AttackCooldown <= 0 && IsAlive(); }
	void SetPosition(const HexCoordinate& NewPos) { Position = NewPos; }

	void MoveTowardAttackRange(const HexCoordinate& Target, int Range, const HexGrid& Grid, const std::unordered_set<HexCoordinate>& Obstacles);
	bool TryAttack(Unit& Target);
	void TakeDamage(int Damage);
	void UpdateCooldowns();
	
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
