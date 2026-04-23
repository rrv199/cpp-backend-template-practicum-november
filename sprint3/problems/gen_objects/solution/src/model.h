#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include <optional>
#include <random>

namespace model {

// Тип трофея на карте
struct LootType {
    std::string name;
    // Другие поля для фронтенда
};

// Карта игрового мира
class Map {
public:
    Map(std::string id, std::string name, size_t loot_types_count)
        : id_(std::move(id))
        , name_(std::move(name))
        , loot_types_count_(loot_types_count) {
    }
    
    const std::string& GetId() const { return id_; }
    const std::string& GetName() const { return name_; }
    size_t GetLootTypesCount() const { return loot_types_count_; }
    
private:
    std::string id_;
    std::string name_;
    size_t loot_types_count_;
};

// Потерянный объект на карте
class LostObject {
public:
    LostObject(int id, int type, double x, double y)
        : id_(id), type_(type), x_(x), y_(y) {}
    
    int GetId() const { return id_; }
    int GetType() const { return type_; }
    double GetX() const { return x_; }
    double GetY() const { return y_; }
    
private:
    int id_;
    int type_;
    double x_;
    double y_;
};

// Игровая сессия
class GameSession {
public:
    explicit GameSession(const Map& map) : map_(map), next_loot_id_(0) {}
    
    const Map& GetMap() const { return map_; }
    
    void AddLostObject(int type, double x, double y) {
        lost_objects_.emplace_back(next_loot_id_++, type, x, y);
    }
    
    const std::vector<LostObject>& GetLostObjects() const {
        return lost_objects_;
    }
    
    size_t GetLootersCount() const { return 1; } // Упрощённо: всегда 1 мародёр
    
private:
    const Map& map_;
    std::vector<LostObject> lost_objects_;
    int next_loot_id_;
};

} // namespace model
