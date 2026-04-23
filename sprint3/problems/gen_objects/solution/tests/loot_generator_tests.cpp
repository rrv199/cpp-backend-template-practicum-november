#include <catch2/catch_test_macros.hpp>
#include <chrono>
#include "../src/loot_generator.h"

using namespace loot_gen;
using namespace std::chrono_literals;

class TestRandomGenerator {
public:
    TestRandomGenerator(std::vector<double> values) : values_(std::move(values)) {}
    
    double operator()() {
        double val = values_[index_ % values_.size()];
        ++index_;
        return val;
    }
    
private:
    std::vector<double> values_;
    size_t index_ = 0;
};

TEST_CASE("LootGenerator initialization", "[loot_generator]") {
    LootGenerator gen(5s, 0.5);
    CHECK(gen.Generate(0ms, 0, 1) == 0);
}

TEST_CASE("LootGenerator with zero looters", "[loot_generator]") {
    LootGenerator gen(5s, 0.5);
    
    SECTION("No looters - no loot generated") {
        CHECK(gen.Generate(100s, 0, 0) == 0);
        CHECK(gen.Generate(100s, 5, 0) == 0);
    }
}

TEST_CASE("LootGenerator respects max loot limit", "[loot_generator]") {
    LootGenerator gen(1s, 1.0);
    
    SECTION("Cannot exceed looter count") {
        unsigned result = gen.Generate(1s, 0, 1);
        CHECK(result <= 1);
        CHECK(gen.Generate(1s, 1, 1) == 0);
    }
    
    SECTION("With multiple looters") {
        CHECK(gen.Generate(1s, 0, 5) <= 5);
        CHECK(gen.Generate(1s, 5, 5) == 0);
    }
}

TEST_CASE("LootGenerator probability", "[loot_generator]") {
    SECTION("Zero probability - never generates") {
        LootGenerator gen(1s, 0.0);
        CHECK(gen.Generate(1000s, 0, 1) == 0);
    }
    
    SECTION("Full probability - always generates when possible") {
        LootGenerator gen(1s, 1.0);
        unsigned result = gen.Generate(1ms, 0, 1);
        CHECK(result == 1);
        CHECK(gen.Generate(1ms, 1, 1) == 0);
    }
}

TEST_CASE("LootGenerator edge cases", "[loot_generator]") {
    SECTION("Very large time delta") {
        LootGenerator gen(1s, 0.5);
        unsigned result = gen.Generate(1000000s, 0, 1);
        CHECK(result <= 1);
    }
    
    SECTION("Very small time delta") {
        LootGenerator gen(1000s, 0.5);
        CHECK(gen.Generate(1ms, 0, 1) == 0);
    }
}
