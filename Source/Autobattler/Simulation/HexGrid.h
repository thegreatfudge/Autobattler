#pragma once
#include <unordered_set>

#include "HexCoordinate.h"

struct PathNode
{
	HexCoordinate Coord;
	float ScoreF;
        
	bool operator>(const PathNode& Other) const
	{
		return ScoreF > Other.ScoreF;
	}
};

class HexGrid
{
public:
	HexGrid(int Radius = 10) : Radius(Radius) {}

	int GetRadius() const { return Radius; }

	bool IsValidCoordinate(const HexCoordinate& Hex) const;

	std::vector<HexCoordinate> GetAllValidCoords() const;
	HexCoordinate GetNextStepTowardRange(const HexCoordinate& From, const HexCoordinate& To, int Range, const std::unordered_set<HexCoordinate>& Obstacles = {}) const;
	std::vector<HexCoordinate> FindPath(const HexCoordinate& Start, const HexCoordinate& Goal, const std::unordered_set<HexCoordinate>& Obstacles = {}, bool bAllowPartialPath = true) const;
	static std::vector<HexCoordinate> ReconstructPath(const std::unordered_map<HexCoordinate, HexCoordinate>& CameFrom, const HexCoordinate& Start, const HexCoordinate& Goal);
	HexCoordinate GetNextStepToward(const HexCoordinate& From, const HexCoordinate& To, const std::unordered_set<HexCoordinate>& Obstacles = {}) const;
	void InvalidatePathCache() const;
	
private:
	int Radius;
	mutable std::unordered_map<std::pair<HexCoordinate, HexCoordinate>, std::vector<HexCoordinate>> PathCache;
	mutable int CacheGeneration = 0;
};
