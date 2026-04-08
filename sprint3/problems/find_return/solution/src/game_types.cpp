#include "game_types.h"

namespace game {

Player::Player(size_t bag_capacity) 
    : bag_capacity_(bag_capacity) {
}

void Player::AddToBag(const Item& item) {
    if (!IsBagFull()) {
        bag_.push_back(item);
    }
}

void Player::ClearBag() {
    bag_.clear();
}

} // namespace game
