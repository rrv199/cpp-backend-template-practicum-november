#include <catch2/catch_test_macros.hpp>
#include "../src/dog_manager.hpp"
#include "../src/database_interface.hpp"
#include <thread>
#include <chrono>

// Мок для базы данных (не требует реального подключения)
class MockDatabase : public database::IDatabaseManager {
public:
    void InitializeTables() override {}
    
    void AddRetiredPlayer(const std::string& name, int score, double play_time_seconds) override {
        last_name = name;
        last_score = score;
        last_play_time = play_time_seconds;
        added_count++;
    }
    
    std::vector<database::RetiredPlayer> GetRecords(int start, int max_items) override {
        return {};
    }
    
    std::string last_name;
    int last_score = 0;
    double last_play_time = 0;
    int added_count = 0;
};

TEST_CASE("Dog inactivity detection", "[dog]") {
    
    SECTION("Dog stops and retires after timeout") {
        MockDatabase db;
        game::DogManager manager(db);
        manager.SetRetirementTime(1.0);
        
        manager.OnDogJoin("dog1", "Rex");
        manager.OnDogMove("dog1");
        manager.OnDogScore("dog1", 100);
        manager.OnDogStop("dog1");
        
        std::this_thread::sleep_for(std::chrono::milliseconds(1100));
        manager.Update();
        
        REQUIRE(db.added_count == 1);
        CHECK(db.last_name == "Rex");
        CHECK(db.last_score == 100);
    }
    
    SECTION("Dog moving does not retire") {
        MockDatabase db;
        game::DogManager manager(db);
        manager.SetRetirementTime(1.0);
        
        manager.OnDogJoin("dog1", "Rex");
        manager.OnDogMove("dog1");
        
        std::this_thread::sleep_for(std::chrono::milliseconds(1100));
        manager.Update();
        
        REQUIRE(db.added_count == 0);
    }
    
    SECTION("Dog that never moved does not retire") {
        MockDatabase db;
        game::DogManager manager(db);
        manager.SetRetirementTime(1.0);
        
        manager.OnDogJoin("dog1", "Rex");
        
        std::this_thread::sleep_for(std::chrono::milliseconds(1100));
        manager.Update();
        
        REQUIRE(db.added_count == 0);
    }
    
    SECTION("Multiple dogs - only inactive retires") {
        MockDatabase db;
        game::DogManager manager(db);
        manager.SetRetirementTime(1.0);
        
        manager.OnDogJoin("dog1", "Rex");
        manager.OnDogMove("dog1");
        manager.OnDogStop("dog1");
        
        manager.OnDogJoin("dog2", "Buddy");
        manager.OnDogMove("dog2");
        
        std::this_thread::sleep_for(std::chrono::milliseconds(1100));
        manager.Update();
        
        REQUIRE(db.added_count == 1);
        CHECK(db.last_name == "Rex");
    }
}
