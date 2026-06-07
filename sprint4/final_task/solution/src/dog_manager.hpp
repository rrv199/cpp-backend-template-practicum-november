#pragma once

#include "database.hpp"
#include <chrono>
#include <unordered_map>
#include <mutex>
#include <string>

namespace game {

struct DogInfo {
    std::string name;
    int score;
    std::chrono::steady_clock::time_point join_time;
    std::chrono::steady_clock::time_point last_movement_time;
    bool is_moving;
    double total_play_time;
};

class DogManager {
public:
    explicit DogManager(database::DatabaseManager& db_manager);
    
    void SetRetirementTime(double seconds);
    
    void OnDogJoin(const std::string& dog_id, const std::string& name);
    void OnDogMove(const std::string& dog_id);
    void OnDogStop(const std::string& dog_id);
    void OnDogScore(const std::string& dog_id, int score_delta);
    
    void Update();
    void CheckInactiveDogs();
    
    void OnDogRetire(const std::string& dog_id);
    
private:
    database::DatabaseManager& db_manager_;
    double retirement_time_seconds_ = 60.0;
    std::unordered_map<std::string, DogInfo> dogs_;
    std::mutex mutex_;
};

} // namespace game
