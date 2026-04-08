#pragma once

#include "game_types.h"
#include <vector>
#include <functional>

namespace game {

// Событие коллизии
struct CollisionEvent {
    double time;           // время события (0-1)
    size_t player_id;      // ID игрока
    size_t item_id;        // ID предмета (для предметов)
    bool is_base;          // true если это база, false если предмет
};

class CollisionHandler {
public:
    CollisionHandler(double time_delta);
    
    // Добавляет игрока для обработки
    void AddPlayer(size_t id, const Player& player, const Point& start_pos);
    
    // Добавляет предмет
    void AddItem(size_t id, const Item& item);
    
    // Добавляет базу
    void AddBase(const Base& base);
    
    // Находит все события коллизий в хронологическом порядке
    std::vector<CollisionEvent> FindCollisions();
    
    // Применяет события к игрокам и предметам
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
