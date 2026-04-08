#pragma once
#include "point.h"
#include "speed.h"
#include "direction.h"
#include "tagged.h"
#include <string>
namespace model {
namespace detail { struct DogTag {}; }
using DogId = util::Tagged<uint64_t, detail::DogTag>;
class Dog {
public:
    Dog(DogId id, const std::string& name, const Point& position)
        : id_(id), name_(name), position_(position), speed_(), direction_(Direction::North) {}
    DogId GetId() const { return id_; }
    const std::string& GetName() const { return name_; }
    const Point& GetPosition() const { return position_; }
    const Speed& GetSpeed() const { return speed_; }
    Direction GetDirection() const { return direction_; }
    void SetPosition(const Point& pos) { position_ = pos; }
private:
    DogId id_;
    std::string name_;
    Point position_;
    Speed speed_;
    Direction direction_;
};
}
