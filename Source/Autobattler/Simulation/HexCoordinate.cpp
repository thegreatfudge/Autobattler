#include "HexCoordinate.h"

#include <array>

int HexCoordinate::DistanceTo(const HexCoordinate& Other) const
{
	return (std::abs(q - Other.q) + std::abs(q + r - Other.q - Other.r) + std::abs(r - Other.r)) / 2;
}

std::array<HexCoordinate, 6> HexCoordinate::GetNeigbhours() const
{
	static std::array<HexCoordinate, 6> Directions{
	{
		{ 1,  0}, { 1, -1}, { 0, -1},
		{-1,  0}, {-1,  1}, { 0,  1}
	  }
	};
        
	std::array<HexCoordinate, 6> Neighbours{};
	for (std::size_t i = 0; i < Directions.size(); ++i)
	{
		Neighbours[i] = *this + Directions[i];
	}
	
	return Neighbours;
}
