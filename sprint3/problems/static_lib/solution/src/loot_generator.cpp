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
    if (looter_count == 0 || loot_count >= looter_count) {
        return 0;
    }
    
    double intervals = static_cast<double>(time_delta.count()) / base_interval_.count();
    double prob = probability_ * intervals;
    if (prob > 1.0) {
        prob = 1.0;
    }
    
    double random_value = random_gen_();
    return (random_value < prob) ? 1 : 0;
}

} // namespace loot_gen
