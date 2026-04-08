#include "logger.h"
#include <thread>
#include <vector>
#include <iostream>

int main() {
    // Тест 1: Простое логирование
    Logger::GetInstance().Log("Hello, world!"s);
    Logger::GetInstance().Log("Number: "s, 42, " and pi: "s, 3.14159);
    
    // Тест 2: Логирование с фиксированным временем
    // Устанавливаем тестовую дату: 2024-03-15 12:34:56
    std::tm test_tm = {};
    test_tm.tm_year = 2024 - 1900;
    test_tm.tm_mon = 3 - 1;
    test_tm.tm_mday = 15;
    test_tm.tm_hour = 12;
    test_tm.tm_min = 34;
    test_tm.tm_sec = 56;
    std::time_t test_time = std::mktime(&test_tm);
    
    Logger::GetInstance().SetTimestamp(test_time);
    Logger::GetInstance().Log("Test log with fixed timestamp");
    
    // Тест 3: Многопоточное логирование
    std::vector<std::thread> threads;
    for (int i = 0; i < 5; ++i) {
        threads.emplace_back([i]() {
            for (int j = 0; j < 10; ++j) {
                Logger::GetInstance().Log("Thread ", i, " message ", j);
            }
        });
    }
    
    for (auto& t : threads) {
        t.join();
    }
    
    std::cout << "All logs written. Check /var/log/sample_log_*.log" << std::endl;
    
    return 0;
}
