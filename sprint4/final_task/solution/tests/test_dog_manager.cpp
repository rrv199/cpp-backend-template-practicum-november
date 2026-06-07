#include <catch2/catch_test_macros.hpp>
#include "../src/dog_manager.hpp"
#include "../src/database.hpp"
#include <thread>
#include <chrono>

class MockDatabase : public database::DatabaseManager {
public:
    MockDatabase() : database::DatabaseManager("postgres://mock") {}
    
    void AddRetiredPlayer(const std::string& name, int score, double play_time_seconds) override {
        last_name = name;
        last_score = score;
        last_play_time = play_time_seconds;
        added_count++;
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
        manager.OnDogStop("dog1");
        
        std::this_thread::sleep_for(std::chrono::milliseconds(1100));
        manager.Update();
        
        REQUIRE(db.added_count == 1);
        CHECK(db.last_name == "Rex");
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
}
