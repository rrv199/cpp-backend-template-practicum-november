#include <catch2/catch_test_macros.hpp>
#include "../src/loot_generator.h"
#include "../src/model.h"
#include <chrono>

using namespace loot_gen;
using namespace model;
using namespace std::chrono_literals;

TEST_CASE("Static library linkage test", "[static-lib]") {
    SECTION("LootGenerator can be created") {
        LootGenerator gen(5s, 0.5);
        CHECK(gen.Generate(0ms, 0, 1) == 0);
    }
    
    SECTION("Map can be created") {
        Map map("test", "Test Map", 5);
        CHECK(map.GetId() == "test");
        CHECK(map.GetName() == "Test Map");
        CHECK(map.GetLootTypesCount() == 5);
    }
    
    SECTION("GameSession can be created") {
        Map map("test", "Test", 3);
        GameSession session(map);
        CHECK(session.GetLostObjects().empty());
        
        session.AddLostObject(0, 10.0, 20.0);
        CHECK(session.GetLostObjects().size() == 1);
        CHECK(session.GetLostObjects()[0].GetType() == 0);
        CHECK(session.GetLostObjects()[0].GetX() == 10.0);
    }
}
