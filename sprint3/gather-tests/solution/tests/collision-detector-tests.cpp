#include <catch2/catch_test_macros.hpp>
#include <catch2/catch_approx.hpp>
#include "../src/collision-detector.h"
#include <algorithm>
#include <cmath>

using namespace collision_detector;
using Catch::Approx;

const double EPS = 1e-10;

// ============================================================
// Тестовый провайдер
// ============================================================
class TestProvider : public ItemGathererProvider {
public:
    void AddItem(Item item) {
        items_.push_back(std::move(item));
    }
    
    void AddGatherer(Gatherer gatherer) {
        gatherers_.push_back(std::move(gatherer));
    }
    
    size_t ItemsCount() const override {
        return items_.size();
    }
    
    Item GetItem(size_t idx) const override {
        return items_[idx];
    }
    
    size_t GatherersCount() const override {
        return gatherers_.size();
    }
    
    Gatherer GetGatherer(size_t idx) const override {
        return gatherers_[idx];
    }
    
private:
    std::vector<Item> items_;
    std::vector<Gatherer> gatherers_;
};

// ============================================================
// Вспомогательные функции
// ============================================================
Item MakeItem(double x, double y, double width = 0.6) {
    return Item{geom::Point2D{x, y}, width};
}

Gatherer MakeGatherer(double x1, double y1, double x2, double y2, double width = 0.6) {
    return Gatherer{geom::Point2D{x1, y1}, geom::Point2D{x2, y2}, width};
}

// ============================================================
// Тест 1: Пустые списки
// ============================================================
TEST_CASE("FindGatherEvents with empty provider", "[collision]") {
    TestProvider provider;
    auto events = FindGatherEvents(provider);
    CHECK(events.empty());
}

// ============================================================
// Тест 2: Нет перемещения
// ============================================================
TEST_CASE("FindGatherEvents with zero movement", "[collision]") {
    TestProvider provider;
    provider.AddGatherer(MakeGatherer(0, 0, 0, 0, 0.6));
    provider.AddItem(MakeItem(0.3, 0, 0.6));
    
    auto events = FindGatherEvents(provider);
    CHECK(events.empty());
}

// ============================================================
// Тест 3: Прямое попадание
// ============================================================
TEST_CASE("FindGatherEvents - direct hit", "[collision]") {
    TestProvider provider;
    provider.AddGatherer(MakeGatherer(0, 0, 10, 0, 0.6));
    provider.AddItem(MakeItem(5, 0, 0.6));
    
    auto events = FindGatherEvents(provider);
    
    REQUIRE(events.size() == 1);
    CHECK(events[0].gatherer_id == 0);
    CHECK(events[0].item_id == 0);
    CHECK(events[0].time == Approx(0.5).epsilon(EPS));
    CHECK(events[0].sq_distance == Approx(0.0).epsilon(EPS));
}

// ============================================================
// Тест 4: Предмет сбоку, но в пределах ширины
// ============================================================
TEST_CASE("FindGatherEvents - item to the side within width", "[collision]") {
    TestProvider provider;
    // Радиус = 0.3, сумма радиусов = 0.6
    provider.AddGatherer(MakeGatherer(0, 0, 10, 0, 0.6));
    provider.AddItem(MakeItem(5, 0.5, 0.6));
    
    auto events = FindGatherEvents(provider);
    
    // Расстояние 0.5 < 0.6, должно быть столкновение
    REQUIRE(events.size() == 1);
    CHECK(events[0].time == Approx(0.5).epsilon(EPS));
}

// ============================================================
// Тест 5: Предмет слишком далеко
// ============================================================
TEST_CASE("FindGatherEvents - item too far", "[collision]") {
    TestProvider provider;
    provider.AddGatherer(MakeGatherer(0, 0, 10, 0, 0.6));
    provider.AddItem(MakeItem(5, 0.7, 0.6));
    
    auto events = FindGatherEvents(provider);
    
    // Расстояние 0.7 > 0.6, столкновения нет
    CHECK(events.empty());
}

// ============================================================
// Тест 6: Предмет перед стартом
// ============================================================
TEST_CASE("FindGatherEvents - item before start", "[collision]") {
    TestProvider provider;
    provider.AddGatherer(MakeGatherer(5, 0, 10, 0, 0.6));
    provider.AddItem(MakeItem(3, 0, 0.6));
    
    auto events = FindGatherEvents(provider);
    
    // Предмет позади старта, столкновения нет
    CHECK(events.empty());
}

// ============================================================
// Тест 7: Предмет после финиша
// ============================================================
TEST_CASE("FindGatherEvents - item after finish", "[collision]") {
    TestProvider provider;
    provider.AddGatherer(MakeGatherer(0, 0, 5, 0, 0.6));
    provider.AddItem(MakeItem(7, 0, 0.6));
    
    auto events = FindGatherEvents(provider);
    
    // Предмет впереди финиша, столкновения нет
    CHECK(events.empty());
}

// ============================================================
// Тест 8: Вертикальное движение
// ============================================================
TEST_CASE("FindGatherEvents - vertical movement", "[collision]") {
    TestProvider provider;
    provider.AddGatherer(MakeGatherer(0, 0, 0, 10, 0.6));
    provider.AddItem(MakeItem(0, 5, 0.6));
    
    auto events = FindGatherEvents(provider);
    
    REQUIRE(events.size() == 1);
    CHECK(events[0].time == Approx(0.5).epsilon(EPS));
}

// ============================================================
// Тест 9: Диагональное движение
// ============================================================
TEST_CASE("FindGatherEvents - diagonal movement", "[collision]") {
    TestProvider provider;
    provider.AddGatherer(MakeGatherer(0, 0, 10, 10, 0.6));
    provider.AddItem(MakeItem(5, 5, 0.6));
    
    auto events = FindGatherEvents(provider);
    
    REQUIRE(events.size() == 1);
    CHECK(events[0].time == Approx(0.5).epsilon(EPS));
}

// ============================================================
// Тест 10: Несколько собирателей и предметов
// ============================================================
TEST_CASE("FindGatherEvents - multiple gatherers and items", "[collision]") {
    TestProvider provider;
    
    // Собиратели
    provider.AddGatherer(MakeGatherer(0, 0, 10, 0, 0.6));      // 0
    provider.AddGatherer(MakeGatherer(0, 2, 10, 2, 0.6));      // 1
    
    // Предметы
    provider.AddItem(MakeItem(3, 0, 0.6));   // 0 - на пути первого
    provider.AddItem(MakeItem(7, 0, 0.6));   // 1 - на пути первого
    provider.AddItem(MakeItem(5, 2, 0.6));   // 2 - на пути второго
    
    auto events = FindGatherEvents(provider);
    
    // Должно быть 3 события
    REQUIRE(events.size() == 3);
    
    // Проверяем индексы
    bool has_g0_i0 = false, has_g0_i1 = false, has_g1_i2 = false;
    for (const auto& e : events) {
        if (e.gatherer_id == 0 && e.item_id == 0) has_g0_i0 = true;
        if (e.gatherer_id == 0 && e.item_id == 1) has_g0_i1 = true;
        if (e.gatherer_id == 1 && e.item_id == 2) has_g1_i2 = true;
    }
    CHECK(has_g0_i0);
    CHECK(has_g0_i1);
    CHECK(has_g1_i2);
}

// ============================================================
// Тест 11: Хронологический порядок
// ============================================================
TEST_CASE("FindGatherEvents - chronological order", "[collision]") {
    TestProvider provider;
    
    provider.AddGatherer(MakeGatherer(0, 0, 10, 0, 0.6));
    provider.AddItem(MakeItem(2, 0, 0.6));   // время 0.2
    provider.AddItem(MakeItem(8, 0, 0.6));   // время 0.8
    provider.AddItem(MakeItem(5, 0, 0.6));   // время 0.5
    
    auto events = FindGatherEvents(provider);
    
    REQUIRE(events.size() == 3);
    
    // Проверяем, что события отсортированы по времени
    CHECK(events[0].time <= events[1].time);
    CHECK(events[1].time <= events[2].time);
}

// ============================================================
// Тест 12: Разные размеры предметов и собирателей
// ============================================================
TEST_CASE("FindGatherEvents - different widths", "[collision]") {
    TestProvider provider;
    
    // Собиратель с шириной 1.0 (радиус 0.5)
    provider.AddGatherer(MakeGatherer(0, 0, 10, 0, 1.0));
    // Предмет с шириной 0.4 (радиус 0.2)
    // Сумма радиусов = 0.7
    provider.AddItem(MakeItem(5, 0.65, 0.4));
    
    auto events = FindGatherEvents(provider);
    
    // 0.65 < 0.7 -> столкновение есть
    REQUIRE(events.size() == 1);
}

// ============================================================
// Тест 13: Нет столкновения из-за ширины
// ============================================================
TEST_CASE("FindGatherEvents - no collision due to width", "[collision]") {
    TestProvider provider;
    
    provider.AddGatherer(MakeGatherer(0, 0, 10, 0, 0.6));
    provider.AddItem(MakeItem(5, 0.65, 0.4));
    
    auto events = FindGatherEvents(provider);
    
    // Сумма радиусов = 0.3 + 0.2 = 0.5
    // Расстояние 0.65 > 0.5 -> столкновения нет
    CHECK(events.empty());
}

// ============================================================
// Тест 14: Обратное движение (end_x < start_x)
// ============================================================
TEST_CASE("FindGatherEvents - reverse movement", "[collision]") {
    TestProvider provider;
    provider.AddGatherer(MakeGatherer(10, 0, 0, 0, 0.6));
    provider.AddItem(MakeItem(5, 0, 0.6));
    
    auto events = FindGatherEvents(provider);
    
    REQUIRE(events.size() == 1);
    CHECK(events[0].time == Approx(0.5).epsilon(EPS));
}
