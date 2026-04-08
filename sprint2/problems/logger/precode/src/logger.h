#pragma once

#include <chrono>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <mutex>
#include <sstream>
#include <string>
#include <string_view>
#include <ctime>

using namespace std::literals;

class Logger {
public:
    // Запрещаем копирование
    Logger(const Logger&) = delete;
    Logger& operator=(const Logger&) = delete;
    
    // Получение единственного экземпляра
    static Logger& GetInstance() {
        static Logger instance;
        return instance;
    }
    
    // Установка тестового времени
    void SetTimestamp(std::time_t timestamp) {
        std::lock_guard<std::mutex> lock(mutex_);
        use_fixed_time_ = true;
        fixed_time_ = timestamp;
    }
    
    // Основной метод логирования с variadic templates
    template <typename... Args>
    void Log(Args&&... args) {
        std::lock_guard<std::mutex> lock(mutex_);
        
        // Получаем текущую дату
        std::time_t now = use_fixed_time_ ? fixed_time_ : std::time(nullptr);
        std::tm* tm_now = std::gmtime(&now);
        
        // Проверяем, нужно ли сменить файл
        if (current_year_ != tm_now->tm_year || current_month_ != tm_now->tm_mon || current_day_ != tm_now->tm_mday) {
            RotateFile(tm_now);
        }
        
        // Форматируем временную метку
        std::string timestamp = FormatTimestamp(now);
        
        // Выводим в файл
        if (log_file_.is_open()) {
            log_file_ << timestamp << ": ";
            WriteToStream(log_file_, std::forward<Args>(args)...);
            log_file_ << std::endl;
            log_file_.flush();
        }
    }
    
private:
    Logger() : log_file_(), current_year_(-1), current_month_(-1), current_day_(-1), 
               use_fixed_time_(false), fixed_time_(0) {}
    
    ~Logger() {
        if (log_file_.is_open()) {
            log_file_.close();
        }
    }
    
    // Форматирование временной метки
    static std::string FormatTimestamp(std::time_t time) {
        std::tm* tm_time = std::gmtime(&time);
        std::ostringstream oss;
        oss << std::put_time(tm_time, "%Y-%m-%d %H:%M:%S");
        return oss.str();
    }
    
    // Получение имени файла для даты
    static std::string GetLogFileName(const std::tm* tm_time) {
        std::ostringstream oss;
        oss << "/var/log/sample_log_"
            << std::setfill('0') << std::setw(4) << (tm_time->tm_year + 1900) << "_"
            << std::setfill('0') << std::setw(2) << (tm_time->tm_mon + 1) << "_"
            << std::setfill('0') << std::setw(2) << tm_time->tm_mday
            << ".log";
        return oss.str();
    }
    
    // Смена файла при смене даты
    void RotateFile(const std::tm* tm_now) {
        if (log_file_.is_open()) {
            log_file_.close();
        }
        
        current_year_ = tm_now->tm_year;
        current_month_ = tm_now->tm_mon;
        current_day_ = tm_now->tm_mday;
        
        std::string filename = GetLogFileName(tm_now);
        log_file_.open(filename, std::ios::app);
        
        if (!log_file_.is_open()) {
            // Если не можем открыть в /var/log, пишем в текущую директорию
            std::string fallback_filename = "sample_log_" +
                std::to_string(tm_now->tm_year + 1900) + "_" +
                std::to_string(tm_now->tm_mon + 1) + "_" +
                std::to_string(tm_now->tm_mday) + ".log";
            log_file_.open(fallback_filename, std::ios::app);
        }
    }
    
    // Базовый случай для рекурсивного вывода
    void WriteToStream(std::ostream& os) {}
    
    // Рекурсивный вывод всех аргументов
    template <typename T, typename... Rest>
    void WriteToStream(std::ostream& os, T&& first, Rest&&... rest) {
        os << std::forward<T>(first);
        WriteToStream(os, std::forward<Rest>(rest)...);
    }
    
    std::ofstream log_file_;
    std::mutex mutex_;
    int current_year_;
    int current_month_;
    int current_day_;
    bool use_fixed_time_;
    std::time_t fixed_time_;
};

// Удобный макрос для логирования (опционально)
#define LOG(...) Logger::GetInstance().Log(__VA_ARGS__)

