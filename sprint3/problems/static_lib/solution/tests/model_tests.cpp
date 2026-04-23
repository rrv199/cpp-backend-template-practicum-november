#include <catch2/catch_test_macros.hpp>
#include "../src/model.h"

using namespace model;

TEST_CASE("Map creation", "[model]") {
    Map map("map1", "Test Map", 3);
    
    CHECK(map.GetId() == "map1");
    CHECK(map.GetName() == "Test Map");
    CHECK(map.GetLootTypesCount() == 3);
}

TEST_CASE("LostObject creation", "[model]") {
    LostObject obj(42, 2, 10.5, 20.0);
    
    CHECK(obj.GetId() == 42);
    CHECK(obj.GetType() == 2);
    CHECK(obj.GetX() == 10.5);
    CHECK(obj.GetY() == 20.0);
}

TEST_CASE("GameSession manages lost objects", "[model]") {
    Map map("map1", "Test", 5);
    GameSession session(map);
    
    SECTION("Initially empty") {
        CHECK(session.GetLostObjects().empty());
    }
    
    SECTION("Can add lost objects") {
        session.AddLostObject(0, 10.0, 20.0);
        session.AddLostObject(1, 30.0, 40.0);
        
        const auto& objects = session.GetLostObjects();
        CHECK(objects.size() == 2);
        CHECK(objects[0].GetType() == 0);
        CHECK(objects[0].GetX() == 10.0);
        CHECK(objects[1].GetType() == 1);
    }
    
    SECTION("Objects have unique IDs") {
        session.AddLostObject(0, 0, 0);
        session.AddLostObject(1, 0, 0);
        
        const auto& objects = session.GetLostObjects();
        CHECK(objects[0].GetId() != objects[1].GetId());
    }
}
