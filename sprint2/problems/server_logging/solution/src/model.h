#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include "tagged.h"

namespace model {

using Dimension = int;
using Coord = Dimension;

struct Point {
    Coord x, y;
};

struct Size {
    Dimension width, height;
};

struct Rectangle {
    Point position;
    Size size;
};

struct Offset {
    Dimension dx, dy;
};

class Road {
public:
    enum Type { Horizontal, Vertical };
    
    Road(Type type, Point start, Point end) : type_(type), start_(start), end_(end) {}
    
    Type GetType() const { return type_; }
    Point GetStart() const { return start_; }
    Point GetEnd() const { return end_; }
    bool IsHorizontal() const { return type_ == Horizontal; }
    bool IsVertical() const { return type_ == Vertical; }

private:
    Type type_;
    Point start_;
    Point end_;
};

class Building {
public:
    explicit Building(Rectangle bounds) : bounds_(bounds) {}
    const Rectangle& GetBounds() const { return bounds_; }

private:
    Rectangle bounds_;
};

class Office {
public:
    using Id = util::Tagged<std::string, Office>;

    Office(Id id, Point position, Offset offset)
        : id_(std::move(id)), position_(position), offset_(offset) {}

    const Id& GetId() const { return id_; }
    Point GetPosition() const { return position_; }
    Offset GetOffset() const { return offset_; }

private:
    Id id_;
    Point position_;
    Offset offset_;
};

class Map {
public:
    using Id = util::Tagged<std::string, Map>;

    Map(Id id, std::string name) : id_(std::move(id)), name_(std::move(name)) {}

    const Id& GetId() const { return id_; }
    const std::string& GetName() const { return name_; }
    const std::vector<Road>& GetRoads() const { return roads_; }
    const std::vector<Building>& GetBuildings() const { return buildings_; }
    const std::vector<Office>& GetOffices() const { return offices_; }

    void AddRoad(const Road& road) { roads_.push_back(road); }
    void AddBuilding(const Building& building) { buildings_.push_back(building); }
    void AddOffice(const Office& office) { offices_.push_back(office); }

private:
    Id id_;
    std::string name_;
    std::vector<Road> roads_;
    std::vector<Building> buildings_;
    std::vector<Office> offices_;
};

class Game {
public:
    void AddMap(Map map) { maps_.push_back(std::move(map)); }
    const std::vector<Map>& GetMaps() const { return maps_; }
    const Map* FindMap(const Map::Id& id) const {
        for (const auto& map : maps_) {
            if (*map.GetId() == *id) return &map;
        }
        return nullptr;
    }

private:
    std::vector<Map> maps_;
};

} // namespace model
