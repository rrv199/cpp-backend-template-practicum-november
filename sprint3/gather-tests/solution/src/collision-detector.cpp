#include "collision-detector.h"
#include <cmath>
#include <algorithm>
#include <vector>

namespace collision_detector {

namespace {
    // Вспомогательная структура для хранения проекции
    struct ProjectionResult {
        double proj_ratio;      // Доля пройденного пути до ближайшей точки
        double sq_distance;     // Квадрат расстояния до отрезка
        bool is_on_segment;     // Попадает ли проекция на отрезок
    };
    
    // Вычисляет проекцию точки C на отрезок AB
    ProjectionResult ProjectPointToSegment(
        const geom::Point2D& a, 
        const geom::Point2D& b, 
        const geom::Point2D& c) {
        
        // Вектор AB
        double vx = b.x - a.x;
        double vy = b.y - a.y;
        
        // Квадрат длины AB
        double v_len2 = vx * vx + vy * vy;
        
        // Если отрезок нулевой длины
        if (v_len2 < 1e-15) {
            // Точка считается не на отрезке, расстояния нет
            return {0.0, 0.0, false};
        }
        
        // Вектор AC
        double ux = c.x - a.x;
        double uy = c.y - a.y;
        
        // Скалярное произведение (AC · AB)
        double u_dot_v = ux * vx + uy * vy;
        
        // Доля пройденного пути (от 0 до 1)
        double proj_ratio = u_dot_v / v_len2;
        
        // Ближайшая точка на прямой
        double closest_x, closest_y;
        bool on_segment;
        
        if (proj_ratio <= 0.0) {
            // Ближайшая точка - A
            closest_x = a.x;
            closest_y = a.y;
            on_segment = false;
        } else if (proj_ratio >= 1.0) {
            // Ближайшая точка - B
            closest_x = b.x;
            closest_y = b.y;
            on_segment = false;
        } else {
            // Ближайшая точка внутри отрезка
            closest_x = a.x + proj_ratio * vx;
            closest_y = a.y + proj_ratio * vy;
            on_segment = true;
        }
        
        // Квадрат расстояния от точки C до ближайшей точки
        double dx = c.x - closest_x;
        double dy = c.y - closest_y;
        double sq_distance = dx * dx + dy * dy;
        
        return {proj_ratio, sq_distance, on_segment};
    }
    
    // Проверяет, сталкивается ли собиратель с предметом
    bool CheckCollision(const Gatherer& gatherer, const Item& item, 
                        double& out_time, double& out_sq_distance) {
        
        // Радиусы
        double gatherer_radius = gatherer.width / 2.0;
        double item_radius = item.width / 2.0;
        double sum_radius = gatherer_radius + item_radius;
        
        // Проверяем, что перемещение ненулевое
        double dx = gatherer.end_pos.x - gatherer.start_pos.x;
        double dy = gatherer.end_pos.y - gatherer.start_pos.y;
        if (std::abs(dx) < 1e-15 && std::abs(dy) < 1e-15) {
            return false;
        }
        
        // Вычисляем проекцию предмета на отрезок перемещения
        auto proj = ProjectPointToSegment(
            gatherer.start_pos, 
            gatherer.end_pos, 
            item.position
        );
        
        // Проверяем расстояние
        if (proj.sq_distance > sum_radius * sum_radius) {
            return false;
        }
        
        // Проверяем, что проекция попадает на отрезок
        // (или очень близка к нему с учётом погрешности)
        if (!proj.is_on_segment) {
            // Если проекция не на отрезке, но расстояние меньше суммы радиусов,
            // то это возможно только если предмет касается начала или конца
            // движения. В этом случае время = 0 или 1
            double time = (proj.proj_ratio <= 0.0) ? 0.0 : 1.0;
            
            // Проверяем, что расстояние действительно меньше
            if (proj.sq_distance <= sum_radius * sum_radius) {
                out_time = time;
                out_sq_distance = proj.sq_distance;
                return true;
            }
            return false;
        }
        
        // Вычисляем время столкновения
        // Время = доля пути до ближайшей точки
        out_time = proj.proj_ratio;
        out_sq_distance = proj.sq_distance;
        
        return true;
    }
} // anonymous namespace

std::vector<GatheringEvent> FindGatherEvents(const ItemGathererProvider& provider) {
    std::vector<GatheringEvent> events;
    
    size_t num_gatherers = provider.GatherersCount();
    size_t num_items = provider.ItemsCount();
    
    // Перебираем всех собирателей и все предметы
    for (size_t g = 0; g < num_gatherers; ++g) {
        auto gatherer = provider.GetGatherer(g);
        
        for (size_t i = 0; i < num_items; ++i) {
            auto item = provider.GetItem(i);
            
            double time = 0.0;
            double sq_distance = 0.0;
            
            if (CheckCollision(gatherer, item, time, sq_distance)) {
                events.push_back(GatheringEvent{
                    .item_id = i,
                    .gatherer_id = g,
                    .sq_distance = sq_distance,
                    .time = time
                });
            }
        }
    }
    
    // Сортируем события по времени
    std::sort(events.begin(), events.end(),
        [](const GatheringEvent& a, const GatheringEvent& b) {
            return a.time < b.time;
        });
    
    return events;
}

} // namespace collision_detector
