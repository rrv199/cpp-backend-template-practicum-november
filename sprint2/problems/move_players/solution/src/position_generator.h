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
        
        std::uniform_int_distribution<size_t> road_dist(0, roads.size() - 1);
        const auto& road = roads[road_dist(rng_)];
        
        std::uniform_real_distribution<double> coord_dist(0.0, 1.0);
        double t = coord_dist(rng_);
        
        auto start = road.GetStart();
        auto end = road.GetEnd();
        
        if (road.IsHorizontal()) {
            double x = start.x + t * (end.x - start.x);
            return Point(x, start.y);
        } else {
            double y = start.y + t * (end.y - start.y);
            return Point(start.x, y);
        }
    }

private:
    std::mt19937 rng_;
};

}  // namespace model
