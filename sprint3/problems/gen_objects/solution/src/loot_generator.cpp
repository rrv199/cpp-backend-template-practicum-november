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
    if (looter_count == 0) {
        return 0;
    }
    
    unsigned max_allowed_loot = looter_count;
    
    if (loot_count >= max_allowed_loot) {
        return 0;
    }
    
    double intervals = static_cast<double>(time_delta.count()) / base_interval_.count();
    double prob = 1.0 - std::pow(1.0 - probability_, intervals);
    double random_value = random_gen_();
    
    if (random_value < prob) {
        return 1;
    }
    
    return 0;
}

} // namespace loot_gen
