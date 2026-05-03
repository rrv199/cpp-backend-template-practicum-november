#include <catch2/catch_test_macros.hpp>
#include "../src/database.hpp"
#include <cstdlib>

// Для реальных тестов нужна тестовая база данных
// Эти тесты требуют настройки GAME_DB_URL

TEST_CASE("Database operations", "[database]") {
    const char* db_url = std::getenv("GAME_DB_URL");
    if (!db_url) {
        SKIP("GAME_DB_URL not set, skipping database tests");
    }
    
    SECTION("Initialize tables creates table") {
        database::DatabaseManager db(db_url);
        REQUIRE_NOTHROW(db.InitializeTables());
    }
    
    SECTION("Add and retrieve retired player") {
        database::DatabaseManager db(db_url);
        db.InitializeTables();
        
        // Очищаем таблицу
        // db.ClearTable(); - нужно добавить метод
        
        db.AddRetiredPlayer("TestDog", 100, 10.5);
        
        auto records = db.GetRecords(0, 10);
        
        bool found = false;
        for (const auto& r : records) {
            if (r.name == "TestDog" && r.score == 100) {
                found = true;
                CHECK(r.play_time_seconds == Approx(10.5));
            }
        }
        CHECK(found);
    }
    
    SECTION("Pagination works") {
        database::DatabaseManager db(db_url);
        
        // Добавляем несколько записей
        for (int i = 0; i < 5; ++i) {
            db.AddRetiredPlayer("Dog" + std::to_string(i), i * 100, i * 10.0);
        }
        
        auto first_two = db.GetRecords(0, 2);
        CHECK(first_two.size() == 2);
        
        auto next_two = db.GetRecords(2, 2);
        CHECK(next_two.size() == 2);
    }
    
    SECTION("Max items limited to 100") {
        database::DatabaseManager db(db_url);
        auto records = db.GetRecords(0, 200);
        CHECK(records.size() <= 100);
    }
}
