#include "database.hpp"
#include "dog_manager.hpp"
#include "records_handler.hpp"
#include <cstdlib>
#include <iostream>
#include <thread>
#include <chrono>

std::string GetDbUrl() {
    const char* url = std::getenv("GAME_DB_URL");
    if (!url) {
        std::cerr << "GAME_DB_URL environment variable not set" << std::endl;
        exit(EXIT_FAILURE);
    }
    return std::string(url);
}

int main() {
    try {
        auto db_url = GetDbUrl();
        database::DatabaseManager db_manager(db_url);
        db_manager.InitializeTables();
        
        game::DogManager dog_manager(db_manager);
        dog_manager.SetRetirementTime(60.0);
        
        std::cout << "Game server initialized successfully" << std::endl;
        
        // Пример использования
        dog_manager.OnDogJoin("dog1", "Rex");
        dog_manager.OnDogMove("dog1");
        dog_manager.OnDogScore("dog1", 100);
        dog_manager.OnDogStop("dog1");
        
        std::this_thread::sleep_for(std::chrono::seconds(2));
        dog_manager.Update();
        
        return 0;
        
    } catch (const std::exception& e) {
        std::cerr << "Fatal error: " << e.what() << std::endl;
        return EXIT_FAILURE;
    }
}
