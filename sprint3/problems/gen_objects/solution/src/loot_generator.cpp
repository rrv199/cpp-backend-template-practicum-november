#include "loot_generator.h"
#include <cmath>
#include <algorithm>
#include <iostream>

namespace loot_gen {

LootGenerator::LootGenerator(TimeInterval base_interval, double probability,
                             RandomGenerator random_gen)
    : base_interval_(base_interval)
    , probability_(probability)
    , random_gen_(std::move(random_gen)) {
}

unsigned LootGenerator::Generate(TimeInterval time_delta, unsigned loot_count, 
                                  unsigned looter_count) {
    // Если нет мародёров, трофеи не генерируются
    if (looter_count == 0) {
        return 0;
    }
    
    // Максимальное количество трофеев не может превышать количество мародёров
    unsigned max_allowed_loot = looter_count;
    
    // Если трофеев уже максимальное количество, новые не генерируем
    if (loot_count >= max_allowed_loot) {
        return 0;
    }
    
    // Количество интервалов, прошедших за time_delta
    // Например: base_interval = 5s, time_delta = 10s -> intervals = 2
    double intervals = static_cast<double>(time_delta.count()) / base_interval_.count();
    
    // Вероятность появления хотя бы одного трофея за intervals интервалов
    // P = 1 - (1 - probability)^intervals
    double prob = 1.0 - std::pow(1.0 - probability_, intervals);
    
    // Генерируем случайное число от 0 до 1
    double random_value = random_gen_();
    
    // Если random_value < prob, то генерируем 1 трофей
    if (random_value < prob) {
        return 1;
    }
    
    return 0;
}

}  // namespace loot_gen
