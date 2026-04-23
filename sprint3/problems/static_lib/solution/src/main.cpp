#include "loot_generator.h"
#include "model.h"
#include <iostream>
#include <chrono>

using namespace loot_gen;
using namespace model;
using namespace std::chrono_literals;

int main() {
    std::cout << "Game Server Starting..." << std::endl;
    
    Map map("map1", "Test Map", 3);
    GameSession session(map);
    
    LootGenerator generator(5s, 0.5);
    
    std::cout << "Game Server Ready!" << std::endl;
    std::cout << "Map: " << map.GetName() << " (ID: " << map.GetId() << ")" << std::endl;
    std::cout << "Loot types count: " << map.GetLootTypesCount() << std::endl;
    
    return 0;
}
