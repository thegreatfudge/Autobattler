#include "HexGrid.h"

#include <array>
#include <queue>
#include <unordered_set>

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

HexCoordinate HexGrid::GetNextStepTowardRange(const HexCoordinate& From, const HexCoordinate& To, const int Range,
	const std::unordered_set<HexCoordinate>& Obstacles) const
{
	int currentDistance = From.DistanceTo(To);
        
	if (currentDistance <= Range) {
		return From;
	}
        
	std::vector<HexCoordinate> TargetHexes;
	TargetHexes.reserve((2 * Range + 1) * (2 * Range + 1));
        
	for (int q = To.q - Range; q <= To.q + Range; q++)
	{
		for (int r = To.r - Range; r <= To.r + Range; r++)
		{
			if (HexCoordinate Hex(q, r); IsValidCoordinate(Hex) && Hex.DistanceTo(To) <= Range && !Obstacles.contains(Hex))
			{
				TargetHexes.push_back(Hex);
			}
		}
	}
        
	std::vector<HexCoordinate> BestPath;
	int BestPathLength = std::numeric_limits<int>::max();
        
	for (const auto& TargetHex : TargetHexes)
	{
		if (auto Path = FindPath(From, TargetHex, Obstacles, false); !Path.empty() && static_cast<int>(Path.size()) < BestPathLength)
		{
			BestPathLength = static_cast<int>(Path.size());
			BestPath = Path;
		}
	}
        
	if (!BestPath.empty()) {
		return BestPath[0];
	}
        
	return GetNextStepToward(From, To, Obstacles);
}

std::vector<HexCoordinate> HexGrid::FindPath(const HexCoordinate& Start, const HexCoordinate& Goal,
	const std::unordered_set<HexCoordinate>& Obstacles, bool bAllowPartialPath) const
{
	if (!IsValidCoordinate(Start) || !IsValidCoordinate(Goal))
	{
        return {};
    }
        
    if (!bAllowPartialPath && Obstacles.contains(Goal))
    {
        return {};
    }
    

    if (Obstacles.empty())
    {
        auto cacheKey = std::make_pair(Start, Goal);
        auto cached = PathCache.find(cacheKey);
        if (cached != PathCache.end()) {
            return cached->second;
        }
    }
    
    std::priority_queue<PathNode, std::vector<PathNode>, std::greater<PathNode>> OpenSet;
    std::unordered_map<HexCoordinate, float> gScore;
    std::unordered_map<HexCoordinate, HexCoordinate> CameFrom;
    std::unordered_set<HexCoordinate> ClosedSet;
    
    gScore[Start] = 0;
    float hScore = static_cast<float>(Start.DistanceTo(Goal));
    OpenSet.push({Start, hScore});
    
    HexCoordinate BestNode = Start;
    float BestDistance = hScore;
    
    while (!OpenSet.empty())
    {
        PathNode Current = OpenSet.top();
        OpenSet.pop();
        
        if (ClosedSet.contains(Current.Coord))
        {
            continue;
        }
        
        ClosedSet.insert(Current.Coord);
        
        if (Current.Coord == Goal)
        {
            auto Path = ReconstructPath(CameFrom, Start, Goal);
            if (Obstacles.empty())
            {
                PathCache[std::make_pair(Start, Goal)] = Path;
            }
            return Path;
        }

        if (float CurrentDistance = static_cast<float>(Current.Coord.DistanceTo(Goal)); CurrentDistance < BestDistance)
        {
            BestDistance = CurrentDistance;
            BestNode = Current.Coord;
        }
        
        for (const HexCoordinate& Neighbor : Current.Coord.GetNeigbhours())
        {
            if (!IsValidCoordinate(Neighbor)) continue;
            if (ClosedSet.contains(Neighbor)) continue;
            
            if (Obstacles.contains(Neighbor))
            {
                if (Neighbor != Goal || !bAllowPartialPath)
                {
                    continue;
                }
            }
            
            float TentativeGScore = gScore[Current.Coord] + 1.0f;

            if (auto GScoreIt = gScore.find(Neighbor); GScoreIt == gScore.end() || TentativeGScore < GScoreIt->second)
            {
                CameFrom[Neighbor] = Current.Coord;
                gScore[Neighbor] = TentativeGScore;
                
                float h = static_cast<float>(Neighbor.DistanceTo(Goal));
                float f = TentativeGScore + h;
                
                OpenSet.push({Neighbor, f});
            }
        }
    }
    
    if (bAllowPartialPath && BestNode != Start)
    {
        return ReconstructPath(CameFrom, Start, BestNode);
    }
    
    return {};
}

std::vector<HexCoordinate> HexGrid::ReconstructPath(const std::unordered_map<HexCoordinate, HexCoordinate>& CameFrom,
	const HexCoordinate& Start, const HexCoordinate& Goal)
{
	std::vector<HexCoordinate> Path;
	HexCoordinate Current = Goal;
        
	while (Current != Start)
	{
		Path.push_back(Current);
		auto It = CameFrom.find(Current);
		if (It == CameFrom.end())
		{
			break;
		}
		Current = It->second;
	}
        
	std::ranges::reverse(Path);
	return Path;
}

HexCoordinate HexGrid::GetNextStepToward(const HexCoordinate& From, const HexCoordinate& To,
	const std::unordered_set<HexCoordinate>& Obstacles) const
{
	if (const auto Path = FindPath(From, To, Obstacles, true); !Path.empty()) 
	{
		return Path[0];
	}
	return From;
}

void HexGrid::InvalidatePathCache() const
{
	CacheGeneration++;
	if (CacheGeneration % 100 == 0)  // Periodically clear to prevent memory bloat
	{ 
		PathCache.clear();
	}
}
