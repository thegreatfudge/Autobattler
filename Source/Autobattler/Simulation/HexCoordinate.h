#pragma once

struct HexCoordinate
{
	int q, r;

	HexCoordinate(int q = 0, int r = 0) : q(q), r(r) {}

	int s() const { return -q - r; }

	int DistanceTo(const HexCoordinate& Other) const;
	std::array<HexCoordinate, 6> GetNeigbhours() const;

	bool operator==(const HexCoordinate& Other) const { return q == Other.q && r == Other.r; }
	bool operator<(const HexCoordinate& Other) const { if (q != Other.q) return q < Other.q; return r < Other.r; }
	HexCoordinate operator+(const HexCoordinate& Other) const noexcept { return { q + Other.q, r + Other.r };}
};

namespace std {
	template<>
	struct hash<HexCoordinate> {
		size_t operator()(const HexCoordinate& h) const {
			return hash<int>()(h.q) ^ (hash<int>()(h.r) << 1);
		}
	};
    
	// Hash for pair<HexCoordinate, HexCoordinate> for path caching
	template<>
	struct hash<pair<HexCoordinate, HexCoordinate>> {
		size_t operator()(const pair<HexCoordinate, HexCoordinate>& p) const {
			const auto H1 = hash<HexCoordinate>()(p.first);
			const auto H2 = hash<HexCoordinate>()(p.second);
			return H1 ^ (H2 << 1);
		}
	};
}
