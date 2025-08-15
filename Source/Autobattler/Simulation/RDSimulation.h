#pragma once
#include <random>

struct HexCoordinate;

class RDSimulation
{
public:
	RDSimulation(const uint32_t Seed) : Generator(Seed) {}

	void Seed(const uint32_t Seed) { Generator.seed(Seed); }
	int Range(int Min, int Max);
	
	HexCoordinate RandomHexCoordinate(int Radius);
	
private:
	std::mt19937 Generator;
};
