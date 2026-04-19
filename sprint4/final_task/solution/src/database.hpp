#pragma once
#include <pqxx/pqxx>
#include <string>
#include <vector>

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
    
private:
    std::shared_ptr<pqxx::connection> conn_;
};

}
