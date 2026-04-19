#include "database.hpp"
#include "dog_manager.hpp"
#include "records_handler.hpp"
#include <cstdlib>
#include <iostream>

int main() {
    const char* db_url = std::getenv("GAME_DB_URL");
    if (!db_url) {
        std::cerr << "GAME_DB_URL not set" << std::endl;
        return 1;
    }
    
    database::DatabaseManager db_manager(db_url);
    db_manager.InitializeTables();
    
    game::DogManager dog_manager(db_manager);
    dog_manager.SetRetirementTime(60.0);
    
    std::cout << "Game server started" << std::endl;
    return 0;
}
