#pragma once
#include <cmath>
namespace model {
// Point уже определён в model.h, здесь только добавляем функции
inline bool operator==(const Point& lhs, const Point& rhs) {
    return std::fabs(lhs.x - rhs.x) < 1e-9 && std::fabs(lhs.y - rhs.y) < 1e-9;
}
}
