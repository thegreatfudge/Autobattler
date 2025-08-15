#include "Unit.h"

Unit::Unit(const int ID, const HexCoordinate& Pos, const ETeamColor Team, const int HP, const int AttackCooldown) :
	ID(ID),
	HP(HP),
	MaxHP(HP),
	AttackCooldown(0),
	MaxAttackCooldown(AttackCooldown),
	TargetUnitID(-1),
	Position(Pos),
	Team(Team),
	State(EUnitState::Idle)
{
	Name = (Team == ETeamColor::Red ? "R" : "B") + std::to_string(ID);
}