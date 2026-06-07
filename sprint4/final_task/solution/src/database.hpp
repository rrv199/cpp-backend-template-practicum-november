#pragma once

#include <pqxx/pqxx>
#include <memory>
#include <string>
#include <vector>
#include <optional>

namespace database {

struct RetiredPlayer {
    std::string name;
    int score;
    double play_time_seconds;
};

class DatabaseManager {
public:
    explicit DatabaseManager(const std::string& connection_string);
    
    void InitializeTables();
    void AddRetiredPlayer(const std::string& name, int score, double play_time_seconds);
    std::vector<RetiredPlayer> GetRecords(int start, int max_items);
    int GetTotalRecordsCount();
    
private:
    std::shared_ptr<pqxx::connection> conn_;
};

} // namespace database
