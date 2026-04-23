#include <catch2/catch_test_macros.hpp>
#include <catch2/catch_approx.hpp>
#include "../src/collision-detector.h"
#include <algorithm>
#include <cmath>

using namespace collision_detector;
using Catch::Approx;

const double EPS = 1e-10;

class TestProvider : public ItemGathererProvider {
public:
    void AddItem(Item item) { items_.push_back(std::move(item)); }
    void AddGatherer(Gatherer gatherer) { gatherers_.push_back(std::move(gatherer)); }
    
    size_t ItemsCount() const override { return items_.size(); }
    Item GetItem(size_t idx) const override { return items_[idx]; }
    size_t GatherersCount() const override { return gatherers_.size(); }
    Gatherer GetGatherer(size_t idx) const override { return gatherers_[idx]; }
    
private:
    std::vector<Item> items_;
    std::vector<Gatherer> gatherers_;
};

Item MakeItem(double x, double y, double width = 0.6) {
    return Item{geom::Point2D{x, y}, width};
}

Gatherer MakeGatherer(double x1, double y1, double x2, double y2, double width = 0.6) {
    return Gatherer{geom::Point2D{x1, y1}, geom::Point2D{x2, y2}, width};
}

TEST_CASE("FindGatherEvents with empty provider", "[collision]") {
    TestProvider provider;
    auto events = FindGatherEvents(provider);
    CHECK(events.empty());
}

TEST_CASE("FindGatherEvents with zero movement", "[collision]") {
    TestProvider provider;
    provider.AddGatherer(MakeGatherer(0, 0, 0, 0, 0.6));
    provider.AddItem(MakeItem(0.3, 0, 0.6));
    auto events = FindGatherEvents(provider);
    CHECK(events.empty());
}

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

TEST_CASE("FindGatherEvents - multiple gatherers and items", "[collision]") {
    TestProvider provider;
    provider.AddGatherer(MakeGatherer(0, 0, 10, 0, 0.6));
    provider.AddGatherer(MakeGatherer(0, 2, 10, 2, 0.6));
    provider.AddItem(MakeItem(3, 0, 0.6));
    provider.AddItem(MakeItem(7, 0, 0.6));
    provider.AddItem(MakeItem(5, 2, 0.6));
    
    auto events = FindGatherEvents(provider);
    REQUIRE(events.size() == 3);
}

TEST_CASE("FindGatherEvents - chronological order", "[collision]") {
    TestProvider provider;
    provider.AddGatherer(MakeGatherer(0, 0, 10, 0, 0.6));
    provider.AddItem(MakeItem(2, 0, 0.6));
    provider.AddItem(MakeItem(8, 0, 0.6));
    provider.AddItem(MakeItem(5, 0, 0.6));
    
    auto events = FindGatherEvents(provider);
    REQUIRE(events.size() == 3);
    CHECK(events[0].time <= events[1].time);
    CHECK(events[1].time <= events[2].time);
}

TEST_CASE("FindGatherEvents - reverse movement", "[collision]") {
    TestProvider provider;
    provider.AddGatherer(MakeGatherer(10, 0, 0, 0, 0.6));
    provider.AddItem(MakeItem(5, 0, 0.6));
    
    auto events = FindGatherEvents(provider);
    REQUIRE(events.size() == 1);
    CHECK(events[0].time == Approx(0.5).epsilon(EPS));
}
