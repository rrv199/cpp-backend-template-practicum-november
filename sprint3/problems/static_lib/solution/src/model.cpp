#include "model.h"

namespace model {

Map::Map(std::string id, std::string name, size_t loot_types_count)
    : id_(std::move(id)), name_(std::move(name)), loot_types_count_(loot_types_count) {}

const std::string& Map::GetId() const { return id_; }
const std::string& Map::GetName() const { return name_; }
size_t Map::GetLootTypesCount() const { return loot_types_count_; }

LostObject::LostObject(int id, int type, double x, double y)
    : id_(id), type_(type), x_(x), y_(y) {}

int LostObject::GetId() const { return id_; }
int LostObject::GetType() const { return type_; }
double LostObject::GetX() const { return x_; }
double LostObject::GetY() const { return y_; }

GameSession::GameSession(const Map& map) : map_(map), next_loot_id_(0) {}

const Map& GameSession::GetMap() const { return map_; }

void GameSession::AddLostObject(int type, double x, double y) {
    lost_objects_.emplace_back(next_loot_id_++, type, x, y);
}

const std::vector<LostObject>& GameSession::GetLostObjects() const {
    return lost_objects_;
}

size_t GameSession::GetLootersCount() const { return 1; }

} // namespace model
