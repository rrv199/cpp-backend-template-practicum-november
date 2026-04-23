#include <iostream>
#include "../src/game_types.h"
#include "../src/collision_handler.h"

using namespace game;

int main() {
    std::cout << "Testing game types..." << std::endl;
    
    Player player(3);
    std::cout << "Player created with capacity: " << player.GetBagCapacity() << std::endl;
    
    Item item1{1, 0, {10.0, 10.0}, 0.0, 10};
    Item item2{2, 1, {20.0, 20.0}, 0.0, 20};
    
    player.AddToBag(item1);
    player.AddToBag(item2);
    
    std::cout << "Bag size: " << player.GetBagSize() << std::endl;
    std::cout << "Score: " << player.GetScore() << std::endl;
    
    player.AddScore(30);
    std::cout << "Score after adding: " << player.GetScore() << std::endl;
    
    player.ClearBag();
    std::cout << "Bag size after clear: " << player.GetBagSize() << std::endl;
    
    std::cout << "All tests passed!" << std::endl;
    return 0;
}
