#include <catch2/catch_test_macros.hpp>
#include <chrono>
#include "../src/loot_generator.h"

using namespace loot_gen;
using namespace std::chrono_literals;

// Вспомогательный генератор для тестов
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
    LootGenerator gen(1s, 1.0); // 100% вероятность
    
    SECTION("Cannot exceed looter count") {
        // При 1 мародёре максимум 1 трофей
        unsigned result = gen.Generate(1s, 0, 1);
        CHECK(result <= 1);
        
        // Уже достигнут максимум
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
        // При probability = 1.0 всегда генерируем, если есть место
        unsigned result = gen.Generate(1ms, 0, 1);
        CHECK(result == 1);
        CHECK(gen.Generate(1ms, 1, 1) == 0);
    }
}

TEST_CASE("LootGenerator with custom random generator", "[loot_generator]") {
    SECTION("Low random value - should generate when prob > random") {
        // random = 0.1, probability = 0.5, intervals = 10/1 = 10
        // prob = 1 - (1-0.5)^10 = 1 - 0.5^10 ≈ 0.999
        // random=0.1 < 0.999 -> генерируем 1
        TestRandomGenerator rng({0.1});
        LootGenerator gen(1s, 0.5, rng);
        CHECK(gen.Generate(10s, 0, 1) == 1);
    }
    
    SECTION("High random value - should NOT generate when random >= prob") {
        // random = 0.9, probability = 0.5, intervals = 10/1 = 10
        // prob = 1 - (1-0.5)^10 = 1 - 0.5^10 ≈ 0.999
        // random=0.9 < 0.999 -> ВСЁ РАВНО генерируем!
        // Чтобы НЕ генерировать, нужно random >= prob, но prob ≈ 0.999
        // Поэтому random=0.9 всё равно меньше, и тест будет падать
        
        // Правильный подход: используем маленькую intervals
        // Чтобы prob была маленькой
        TestRandomGenerator rng({0.9});
        LootGenerator gen(10s, 0.5, rng);
        // За 1 секунду: intervals = 1/10 = 0.1
        // prob = 1 - (1-0.5)^0.1 ≈ 0.067
        // random=0.9 > 0.067 -> НЕ генерируем
        CHECK(gen.Generate(1s, 0, 1) == 0);
    }
}

TEST_CASE("LootGenerator time intervals", "[loot_generator]") {
    SECTION("No time passed - no generation") {
        LootGenerator gen(10s, 0.5);
        CHECK(gen.Generate(0ms, 0, 1) == 0);
    }
    
    SECTION("Small time - low probability") {
        // base_interval=10s, probability=0.5, time_delta=1s
        // intervals = 1/10 = 0.1
        // prob = 1 - (1-0.5)^0.1 = 1 - 0.5^0.1 ≈ 1 - 0.933 = 0.067
        // random=0.1 > 0.067 -> НЕ генерируем
        TestRandomGenerator rng({0.1});
        LootGenerator gen(10s, 0.5, rng);
        CHECK(gen.Generate(1s, 0, 1) == 0);
    }
    
    SECTION("Long time - high probability") {
        // base_interval=10s, probability=0.5, time_delta=100s
        // intervals = 100/10 = 10
        // prob = 1 - (1-0.5)^10 = 1 - 0.5^10 ≈ 0.999
        // random=0.1 < 0.999 -> генерируем
        TestRandomGenerator rng({0.1});
        LootGenerator gen(10s, 0.5, rng);
        CHECK(gen.Generate(100s, 0, 1) == 1);
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

TEST_CASE("LootGenerator deterministic behavior", "[loot_generator]") {
    SECTION("Same random value produces same result") {
        TestRandomGenerator rng({0.5, 0.5});
        LootGenerator gen(10s, 0.5, rng);
        
        // Два вызова с одинаковыми параметрами дают одинаковый результат
        unsigned result1 = gen.Generate(10s, 0, 1);
        unsigned result2 = gen.Generate(10s, 0, 1);
        CHECK(result1 == result2);
    }
}
