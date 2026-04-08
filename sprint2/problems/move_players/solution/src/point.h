#pragma once
#include <cmath>

namespace model {

struct Point {
    double x = 0.0;
    double y = 0.0;
    
    Point() = default;
    Point(double x_, double y_) : x(x_), y(y_) {}
    
    bool operator==(const Point& other) const {
        return std::fabs(x - other.x) < 1e-9 && std::fabs(y - other.y) < 1e-9;
    }
};

}  // namespace model
