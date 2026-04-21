#pragma once
#include <string>
#include <vector>

namespace database {

struct RetiredPlayer {
    std::string name;
    int score;
    double play_time_seconds;
};

class IDatabaseManager {
public:
    virtual ~IDatabaseManager() = default;
    virtual void InitializeTables() = 0;
    virtual void AddRetiredPlayer(const std::string& name, int score, double play_time_seconds) = 0;
    virtual std::vector<RetiredPlayer> GetRecords(int start, int max_items) = 0;
};

} // namespace database
