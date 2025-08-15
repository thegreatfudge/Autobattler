#include "Unit.h"

#include "HexGrid.h"

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

void Unit::MoveTowardAttackRange(const HexCoordinate& Target, int Range, const HexGrid& Grid,
	const std::unordered_set<HexCoordinate>& Obstacles)
{
	if (!IsAlive()) return;
        
	int Distance = Position.DistanceTo(Target);
	if (Distance <= Range) {
		State = EUnitState::Idle;
		return;
	}

	if (const HexCoordinate NextStep = Grid.GetNextStepTowardRange(Position, Target, Range, Obstacles); NextStep != Position)
	{
		Position = NextStep;
		State = EUnitState::Moving;
	}
	else
	{
		State = EUnitState::Idle;
	}
}

bool Unit::TryAttack(Unit& Target)
{
	if (!CanAttack() || !Target.IsAlive())
	{
		return false;
	}
        
	AttackCooldown = MaxAttackCooldown;
	State = EUnitState::Attacking;
	TargetUnitID = Target.GetId();
	Target.TakeDamage(1);
	return true;
}

void Unit::TakeDamage(int Damage)
{
	if (!IsAlive()) return;
        
	HP -= Damage;
	if (HP <= 0)
	{
		HP = 0;
		State = EUnitState::Dead;
	}
}

void Unit::UpdateCooldowns()
{
	if (AttackCooldown > 0)
	{
		AttackCooldown--;
	}
    
	if (State == EUnitState::Attacking && AttackCooldown == MaxAttackCooldown - 1)
	{
		State = EUnitState::Idle;
		TargetUnitID = -1;
	}
}
