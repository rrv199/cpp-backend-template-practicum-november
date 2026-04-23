#pragma once
#include "geom.h"
#include <vector>
#include <cstddef>

namespace collision_detector {

struct Item {
    geom::Point2D position;
    double width = 0.0;
};

struct Gatherer {
    geom::Point2D start_pos;
    geom::Point2D end_pos;
    double width = 0.0;
};

struct GatheringEvent {
    size_t item_id = 0;
    size_t gatherer_id = 0;
    double sq_distance = 0.0;
    double time = 0.0;
};

class ItemGathererProvider {
protected:
    ~ItemGathererProvider() = default;

public:
    virtual size_t ItemsCount() const = 0;
    virtual Item GetItem(size_t idx) const = 0;
    virtual size_t GatherersCount() const = 0;
    virtual Gatherer GetGatherer(size_t idx) const = 0;
};

std::vector<GatheringEvent> FindGatherEvents(const ItemGathererProvider& provider);

} // namespace collision_detector
