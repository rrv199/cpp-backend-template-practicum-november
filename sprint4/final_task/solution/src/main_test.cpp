#include "database.hpp"
#include "dog_manager.hpp"
#include "records_handler.hpp"
#include <cstdlib>
#include <iostream>
#include <memory>
#include <string>
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
        
        // Устанавливаем маленькое время для теста (3 секунды)
        dog_manager.SetRetirementTime(3.0);
        
        std::cout << "Game server initialized successfully" << std::endl;
        std::cout << "Dog retirement time: 3 seconds (for testing)" << std::endl;
        
        // Тест 1: Собака двигается, потом останавливается
        std::cout << "\n=== Test 1: Dog stops and retires after 3 seconds ===" << std::endl;
        dog_manager.OnDogJoin("dog1", "Rex");
        dog_manager.OnDogMove("dog1");
        dog_manager.OnDogScore("dog1", 100);
        dog_manager.OnDogStop("dog1");
        
        // Ждём 4 секунды (больше retirement_time)
        std::cout << "Waiting 4 seconds for retirement..." << std::endl;
        std::this_thread::sleep_for(std::chrono::seconds(4));
        
        dog_manager.Update();
        
        // Тест 2: Собака двигается, не останавливается - не должна уйти на пенсию
        std::cout << "\n=== Test 2: Dog keeps moving - should not retire ===" << std::endl;
        dog_manager.OnDogJoin("dog2", "Buddy");
        dog_manager.OnDogMove("dog2");
        dog_manager.OnDogScore("dog2", 50);
        
        std::this_thread::sleep_for(std::chrono::seconds(4));
        
        dog_manager.Update();  // Должна остаться, так как двигается
        
        // Тест 3: Добавляем ещё одну собаку, которая останавливается
        std::cout << "\n=== Test 3: Another dog stops and retires ===" << std::endl;
        dog_manager.OnDogJoin("dog3", "Max");
        dog_manager.OnDogMove("dog3");
        dog_manager.OnDogScore("dog3", 200);
        dog_manager.OnDogStop("dog3");
        
        std::this_thread::sleep_for(std::chrono::seconds(4));
        
        dog_manager.Update();
        
        // Выводим все записи
        http::RecordsHandler records_handler(db_manager);
        auto records = records_handler.HandleGetRecords(0, 100);
        std::cout << "\n=== All records ===" << std::endl;
        std::cout << records << std::endl;
        
        return 0;
        
    } catch (const std::exception& e) {
        std::cerr << "Fatal error: " << e.what() << std::endl;
        return EXIT_FAILURE;
    }
}
