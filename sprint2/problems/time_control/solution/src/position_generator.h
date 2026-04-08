#pragma once

#include "point.h"
#include "model.h"
#include <random>

namespace model {

class PositionGenerator {
public:
    PositionGenerator() : rng_(std::random_device{}()) {}

    Point GeneratePosition(const Map& map) {
        const auto& roads = map.GetRoads();
        if (roads.empty()) return Point(0, 0);
        
        const auto& road = roads[0];
        auto start = road.GetStart();
        return Point(start.x, start.y);
    }

private:
    std::mt19937 rng_;
};

}  // namespace model
