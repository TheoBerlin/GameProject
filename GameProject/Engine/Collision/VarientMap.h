#pragma once

#include <utility>
#include <vector>
#include <unordered_map>
#include "CollisionInfo.h"

typedef std::tuple<float, float, float> MapKey;
// ---------------------- Hash for the tuple above ----------------------
struct key_hash : public std::unary_function<MapKey, std::size_t> {
	std::size_t operator()(const MapKey& k) const {
		return ((std::size_t)std::get<0>(k)) ^ ((std::size_t)std::get<1>(k)) ^ ((std::size_t)std::get<2>(k));
	}
};
// ------------------ Equal check for the tuple above -------------------
struct key_equal : public std::binary_function<MapKey, MapKey, bool> {
	bool operator()(const MapKey& v0, const MapKey& v1) const {
		return (std::get<0>(v0) == std::get<0>(v1) &&
			std::get<1>(v0) == std::get<1>(v1) &&
			std::get<2>(v0) == std::get<2>(v1));
	}
};

// ------------------------ Data for the map ----------------------------
struct MapData {
	std::vector<CollisionShapeDrawingData*> shapes;
};

// Unordered map which has a key as a tuple of 3 floats (Scale in x, y, z) and data as a vector of CollisionShapeDrawingData (Shape).
typedef std::unordered_map<const MapKey, MapData, key_hash, key_equal> VarientMap;