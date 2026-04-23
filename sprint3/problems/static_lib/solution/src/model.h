#pragma once
#include <string>
#include <vector>
#include <unordered_map>
#include <optional>
#include <random>

namespace model {

struct LootType {
    std::string name;
};

class Map {
public:
    Map(std::string id, std::string name, size_t loot_types_count);
    
    const std::string& GetId() const;
    const std::string& GetName() const;
    size_t GetLootTypesCount() const;
    
private:
    std::string id_;
    std::string name_;
    size_t loot_types_count_;
};

class LostObject {
public:
    LostObject(int id, int type, double x, double y);
    
    int GetId() const;
    int GetType() const;
    double GetX() const;
    double GetY() const;
    
private:
    int id_;
    int type_;
    double x_;
    double y_;
};

class GameSession {
public:
    explicit GameSession(const Map& map);
    
    const Map& GetMap() const;
    void AddLostObject(int type, double x, double y);
    const std::vector<LostObject>& GetLostObjects() const;
    size_t GetLootersCount() const;
    
private:
    const Map& map_;
    std::vector<LostObject> lost_objects_;
    int next_loot_id_;
};

} // namespace model
