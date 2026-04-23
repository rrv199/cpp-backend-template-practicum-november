#pragma once
#include "game_types.h"
#include <vector>
#include <functional>

namespace game {

struct CollisionEvent {
    double time;
    size_t player_id;
    size_t item_id;
    bool is_base;
};

class CollisionHandler {
public:
    CollisionHandler(double time_delta);
    
    void AddPlayer(size_t id, const Player& player, const Point& start_pos);
    void AddItem(size_t id, const Item& item);
    void AddBase(const Base& base);
    std::vector<CollisionEvent> FindCollisions();
    void ApplyEvents(const std::vector<CollisionEvent>& events,
                     std::vector<Player>& players,
                     std::vector<Item>& items);
    
private:
    double time_delta_;
    
    struct MovingPlayer {
        size_t id;
        Point start_pos;
        Point end_pos;
        double width;
    };
    
    struct StaticItem {
        size_t id;
        Point position;
        double width;
        int value;
        int type;
    };
    
    struct StaticBase {
        Point position;
        double width;
    };
    
    std::vector<MovingPlayer> players_;
    std::vector<StaticItem> items_;
    std::vector<StaticBase> bases_;
};

} // namespace game
