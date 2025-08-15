#pragma once
#include "HexCoordinate.h"

class HexGrid
{
public:
	HexGrid(int Radius = 10) : Radius(Radius) {}

	int GetRadius() const { return Radius; }

	bool IsValidCoordinate(const HexCoordinate& Hex) const;

	std::vector<HexCoordinate> GetAllValidCoords() const;

private:
	int Radius;
};
