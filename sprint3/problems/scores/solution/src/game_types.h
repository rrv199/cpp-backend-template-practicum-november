#pragma once

#include <vector>
#include <optional>
#include <cstddef>

namespace game {

// Точка на карте
struct Point {
    double x = 0.0;
    double y = 0.0;
};

// Предмет на карте
struct Item {
    int id = 0;
    int type = 0;
    Point position;
    double width = 0.0;  // ширина предмета (0 для потерянных вещей)
    int value = 0;       // стоимость предмета (для очков)
};

// База (бюро находок)
struct Base {
    Point position;
    double width = 0.5;  // ширина базы
};

// Игрок (собиратель)
class Player {
public:
    Player(size_t bag_capacity = 3);
    
    const Point& GetPosition() const { return position_; }
    void SetPosition(const Point& pos) { position_ = pos; }
    
    const Point& GetSpeed() const { return speed_; }
    void SetSpeed(const Point& speed) { speed_ = speed; }
    
    size_t GetBagCapacity() const { return bag_capacity_; }
    size_t GetBagSize() const { return bag_.size(); }
    bool IsBagFull() const { return bag_.size() >= bag_capacity_; }
    
    void AddToBag(const Item& item);
    void ClearBag();
    const std::vector<Item>& GetBag() const { return bag_; }
    
    int GetScore() const { return score_; }
    void AddScore(int value) { score_ += value; }
    
    double GetWidth() const { return 0.6; }  // ширина игрока
    double GetRadius() const { return GetWidth() / 2.0; }
    
private:
    Point position_;
    Point speed_;
    size_t bag_capacity_;
    std::vector<Item> bag_;
    int score_ = 0;
};

} // namespace game
