#pragma once

#include <chrono>
#include <functional>
#include <random>

namespace loot_gen {

/*
 *  Генератор трофеев
 */
class LootGenerator {
public:
    using RandomGenerator = std::function<double()>;
    using TimeInterval = std::chrono::milliseconds;

    /*
     * base_interval - базовый отрезок времени > 0
     * probability - вероятность появления трофея в течение базового интервала времени
     * random_generator - генератор псевдослучайных чисел в диапазоне от [0 до 1]
     */
    LootGenerator(TimeInterval base_interval, double probability,
                  RandomGenerator random_gen = DefaultGenerator);

    /*
     * Возвращает количество трофеев, которые должны появиться на карте спустя
     * заданный промежуток времени.
     * Количество трофеев, появляющихся на карте не превышает количество мародёров.
     *
     * time_delta - отрезок времени, прошедший с момента предыдущего вызова Generate
     * loot_count - количество трофеев на карте до вызова Generate
     * looter_count - количество мародёров на карте
     */
    unsigned Generate(TimeInterval time_delta, unsigned loot_count, unsigned looter_count);

    void SetRandomGenerator(RandomGenerator gen) {
        random_gen_ = std::move(gen);
    }

private:
    static double DefaultGenerator() noexcept {
        static std::mt19937 gen{std::random_device{}()};
        static std::uniform_real_distribution<> dist{0.0, 1.0};
        return dist(gen);
    }

    TimeInterval base_interval_;
    double probability_;
    RandomGenerator random_gen_;
};

}  // namespace loot_gen
