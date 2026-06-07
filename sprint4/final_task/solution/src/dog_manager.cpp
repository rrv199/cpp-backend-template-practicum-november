#include "dog_manager.hpp"
#include <iostream>
#include <chrono>
#include <vector>

namespace game {

DogManager::DogManager(database::DatabaseManager& db_manager) : db_manager_(db_manager) {}

void DogManager::SetRetirementTime(double seconds) { 
    retirement_time_seconds_ = seconds; 
}

void DogManager::OnDogJoin(const std::string& dog_id, const std::string& name) {
    std::lock_guard lock(mutex_);
    auto now = std::chrono::steady_clock::now();
    dogs_[dog_id] = {name, 0, now, now, false};
}

void DogManager::OnDogMove(const std::string& dog_id) {
    std::lock_guard lock(mutex_);
    auto it = dogs_.find(dog_id);
    if (it != dogs_.end()) {
        it->second.is_moving = true;
        it->second.last_movement_time = std::chrono::steady_clock::now();
    }
}

void DogManager::OnDogStop(const std::string& dog_id) {
    std::lock_guard lock(mutex_);
    auto it = dogs_.find(dog_id);
    if (it != dogs_.end()) {
        it->second.is_moving = false;
        it->second.last_movement_time = std::chrono::steady_clock::now();
    }
}

void DogManager::OnDogScore(const std::string& dog_id, int score_delta) {
    std::lock_guard lock(mutex_);
    auto it = dogs_.find(dog_id);
    if (it != dogs_.end()) {
        it->second.score += score_delta;
    }
}

void DogManager::Update() {
    std::lock_guard lock(mutex_);
    auto now = std::chrono::steady_clock::now();
    std::vector<std::string> to_retire;
    
    for (auto& [id, info] : dogs_) {
        if (!info.is_moving) {
            auto idle_time = std::chrono::duration<double>(now - info.last_movement_time).count();
            if (idle_time >= retirement_time_seconds_) {
                to_retire.push_back(id);
            }
        }
    }
    
    for (const auto& id : to_retire) {
        auto it = dogs_.find(id);
        if (it != dogs_.end()) {
            double play_time = std::chrono::duration<double>(now - it->second.join_time).count();
            db_manager_.AddRetiredPlayer(it->second.name, it->second.score, play_time);
            dogs_.erase(it);
        }
    }
}

}
