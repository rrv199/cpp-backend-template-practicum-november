#include "collision_handler.h"
#include <cmath>
#include <algorithm>
#include <limits>

namespace game {
namespace {
    // Структура для хранения проекции точки на отрезок
    struct Projection {
        double time;        // время от 0 до 1
        double sq_distance; // квадрат расстояния до отрезка
        bool on_segment;    // попадает ли проекция на отрезок
    };
    
    // Вычисляет проекцию точки C на отрезок AB
    Projection ProjectPointToSegment(const Point& a, const Point& b, const Point& c) {
        double vx = b.x - a.x;
        double vy = b.y - a.y;
        double v_len2 = vx * vx + vy * vy;
        
        if (v_len2 < 1e-15) {
            return {0.0, 0.0, false};
        }
        
        double ux = c.x - a.x;
        double uy = c.y - a.y;
        double u_dot_v = ux * vx + uy * vy;
        double time = u_dot_v / v_len2;
        
        double closest_x, closest_y;
        bool on_segment;
        
        if (time <= 0.0) {
            closest_x = a.x;
            closest_y = a.y;
            on_segment = false;
            time = 0.0;
        } else if (time >= 1.0) {
            closest_x = b.x;
            closest_y = b.y;
            on_segment = false;
            time = 1.0;
        } else {
            closest_x = a.x + time * vx;
            closest_y = a.y + time * vy;
            on_segment = true;
        }
        
        double dx = c.x - closest_x;
        double dy = c.y - closest_y;
        double sq_distance = dx * dx + dy * dy;
        
        return {time, sq_distance, on_segment};
    }
    
    // Проверяет столкновение движущегося объекта с неподвижным
    bool CheckCollision(const Point& start, const Point& end, double radius1,
                        const Point& obj_pos, double radius2,
                        double& out_time, double& out_sq_distance) {
        
        double dx = end.x - start.x;
        double dy = end.y - start.y;
        
        if (std::abs(dx) < 1e-15 && std::abs(dy) < 1e-15) {
            return false;
        }
        
        auto proj = ProjectPointToSegment(start, end, obj_pos);
        double sum_radius = radius1 + radius2;
        
        if (proj.sq_distance <= sum_radius * sum_radius) {
            out_time = proj.time;
            out_sq_distance = proj.sq_distance;
            return true;
        }
        
        return false;
    }
} // anonymous namespace

CollisionHandler::CollisionHandler(double time_delta) : time_delta_(time_delta) {}

void CollisionHandler::AddPlayer(size_t id, const Player& player, const Point& start_pos) {
    Point end_pos = player.GetPosition();
    double dx = end_pos.x - start_pos.x;
    double dy = end_pos.y - start_pos.y;
    if (std::abs(dx) < 1e-15 && std::abs(dy) < 1e-15) {
        return;
    }
    players_.push_back({id, start_pos, end_pos, player.GetWidth()});
}

void CollisionHandler::AddItem(size_t id, const Item& item) {
    items_.push_back({id, item.position, item.width, item.value, item.type});
}

void CollisionHandler::AddBase(const Base& base) {
    bases_.push_back({base.position, base.width});
}

std::vector<CollisionEvent> CollisionHandler::FindCollisions() {
    std::vector<CollisionEvent> events;
    
    for (const auto& player : players_) {
        double player_radius = player.width / 2.0;
        
        for (const auto& item : items_) {
            double item_radius = item.width / 2.0;
            double time = 0.0;
            double sq_dist = 0.0;
            
            if (CheckCollision(player.start_pos, player.end_pos, player_radius,
                               item.position, item_radius, time, sq_dist)) {
                events.push_back(CollisionEvent{
                    .time = time,
                    .player_id = player.id,
                    .item_id = item.id,
                    .is_base = false
                });
            }
        }
        
        for (const auto& base : bases_) {
            double base_radius = base.width / 2.0;
            double time = 0.0;
            double sq_dist = 0.0;
            
            if (CheckCollision(player.start_pos, player.end_pos, player_radius,
                               base.position, base_radius, time, sq_dist)) {
                events.push_back(CollisionEvent{
                    .time = time,
                    .player_id = player.id,
                    .item_id = 0,
                    .is_base = true
                });
            }
        }
    }
    
    std::sort(events.begin(), events.end(),
        [](const CollisionEvent& a, const CollisionEvent& b) {
            return a.time < b.time;
        });
    
    return events;
}

void CollisionHandler::ApplyEvents(const std::vector<CollisionEvent>& events,
                                   std::vector<Player>& players,
                                   std::vector<Item>& items) {
    std::vector<bool> item_collected(items.size(), false);
    
    for (const auto& event : events) {
        if (event.player_id >= players.size()) continue;
        
        auto& player = players[event.player_id];
        
        if (event.is_base) {
            for (const auto& item : player.GetBag()) {
                player.AddScore(item.value);
            }
            player.ClearBag();
        } else {
            if (!player.IsBagFull() && event.item_id < items.size() && 
                !item_collected[event.item_id]) {
                player.AddToBag(items[event.item_id]);
                item_collected[event.item_id] = true;
            }
        }
    }
    
    auto it = std::remove_if(items.begin(), items.end(),
        [&item_collected, i = 0](const auto&) mutable {
            return item_collected[i++];
        });
    items.erase(it, items.end());
}

} // namespace game
