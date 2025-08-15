#include "RDSimulation.h"

#include "HexCoordinate.h"

int RDSimulation::Range(int Min, int Max)
{
	if (Min > Max) std::swap(Min, Max);
	std::uniform_int_distribution Dist(Min, Max);
	return Dist(Generator);
}

HexCoordinate RDSimulation::RandomHexCoordinate(const int Radius)
{
	std::vector<HexCoordinate> ValidCoords;
	for (int q = -Radius; q <= Radius; q++)
	{
		const int R1 = std::max(-Radius, -q - Radius);
		const int R2 = std::min(Radius, -q + Radius);
		for (int r = R1; r <= R2; r++)
		{
			ValidCoords.push_back(HexCoordinate(q, r));
		}
	}
        
	if (!ValidCoords.empty())
	{
		const int Index = Range(0, static_cast<int>(ValidCoords.size()) - 1);
		return ValidCoords[Index];
	}
	return HexCoordinate(0, 0);
}
