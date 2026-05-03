#pragma once
#include <string>
#include <vector>
#include <unordered_map>

namespace model {

class Map {
public:
    using Id = std::string;
    Map(Id id, std::string name) : id_(id), name_(name) {}
    const Id& GetId() const { return id_; }
    const std::string& GetName() const { return name_; }
private:
    Id id_;
    std::string name_;
};

class Game {
public:
    void AddMap(Map map) { maps_.push_back(std::move(map)); }
    const std::vector<Map>& GetMaps() const { return maps_; }
private:
    std::vector<Map> maps_;
};

} // namespace model
