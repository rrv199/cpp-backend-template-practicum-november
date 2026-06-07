#include "database.hpp"
#include "dog_manager.hpp"
#include "records_handler.hpp"
#include <cstdlib>
#include <iostream>
#include <thread>
#include <chrono>

int main() {
    const char* db_url = std::getenv("GAME_DB_URL");
    if (!db_url) {
        std::cerr << "GAME_DB_URL not set" << std::endl;
        return 1;
    }
    
    database::DatabaseManager db_manager(db_url);
    db_manager.InitializeTables();
    
    game::DogManager dog_manager(db_manager);
    dog_manager.SetRetirementTime(3.0);
    
    std::cout << "\n=== Test: Dog retirement ===" << std::endl;
    
    dog_manager.OnDogJoin("dog1", "Rex");
    dog_manager.OnDogMove("dog1");
    dog_manager.OnDogScore("dog1", 100);
    dog_manager.OnDogStop("dog1");
    
    std::cout << "Waiting 4 seconds..." << std::endl;
    std::this_thread::sleep_for(std::chrono::seconds(4));
    
    dog_manager.Update();
    
    auto records = db_manager.GetRecords(0, 10);
    std::cout << "\n=== Retired players ===" << std::endl;
    for (const auto& r : records) {
        std::cout << "Name: " << r.name << ", Score: " << r.score << ", PlayTime: " << r.play_time_seconds << "s" << std::endl;
    }
    
    return 0;
}
