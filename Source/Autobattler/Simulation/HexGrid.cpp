#include "HexGrid.h"

bool HexGrid::IsValidCoordinate(const HexCoordinate& Hex) const
{
	return (std::abs(Hex.q) <= Radius && std::abs(Hex.r) <= Radius && std::abs(Hex.s()) <= Radius);
}

std::vector<HexCoordinate> HexGrid::GetAllValidCoords() const
{
	std::vector<HexCoordinate> coords;
	coords.reserve((2 * Radius + 1) * (2 * Radius + 1));
        
	for (int q = -Radius; q <= Radius; q++)
	{
		const int R1 = std::max(-Radius, -q - Radius);
		const int R2 = std::min(Radius, -q + Radius);
		for (int r = R1; r <= R2; r++)
		{
			coords.push_back(HexCoordinate(q, r));
		}
	}
	return coords;
}
