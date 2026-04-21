#include "loot_generator.h"
#include <cmath>
#include <algorithm>

namespace loot_gen {

LootGenerator::LootGenerator(TimeInterval base_interval, double probability,
                             RandomGenerator random_gen)
    : base_interval_(base_interval)
    , probability_(probability)
    , random_gen_(std::move(random_gen)) {
}

unsigned LootGenerator::Generate(TimeInterval time_delta, unsigned loot_count, 
                                  unsigned looter_count) {
    // Если нет мародёров или уже достигнут максимум трофеев
    if (looter_count == 0 || loot_count >= looter_count) {
        return 0;
    }
    
    // Количество интервалов, прошедших за time_delta
    double intervals = static_cast<double>(time_delta.count()) / base_interval_.count();
    
    // Вероятность появления трофея (линейная)
    double prob = probability_ * intervals;
    if (prob > 1.0) {
        prob = 1.0;
    }
    
    // Генерируем случайное число
    double random_value = random_gen_();
    
    // Возвращаем 1, если выпало, иначе 0
    return (random_value < prob) ? 1 : 0;
}

}  // namespace loot_gen
