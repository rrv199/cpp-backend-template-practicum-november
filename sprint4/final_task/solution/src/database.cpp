#include "database.hpp"
#include <iostream>
#include <algorithm>

namespace database {

DatabaseManager::DatabaseManager(const std::string& connection_string) {
    conn_ = std::make_shared<pqxx::connection>(connection_string);
}

void DatabaseManager::InitializeTables() {
    pqxx::work w(*conn_);
    w.exec("CREATE TABLE IF NOT EXISTS retired_players ("
           "id SERIAL PRIMARY KEY, name VARCHAR(100) NOT NULL, "
           "score INTEGER NOT NULL DEFAULT 0, play_time_seconds DOUBLE PRECISION NOT NULL);");
    w.exec("CREATE INDEX IF NOT EXISTS idx_retired_players_score_time_name "
           "ON retired_players (score DESC, play_time_seconds ASC, name ASC);");
    w.commit();
    std::cout << "Database tables initialized" << std::endl;
}

void DatabaseManager::AddRetiredPlayer(const std::string& name, int score, double play_time_seconds) {
    pqxx::work w(*conn_);
    w.exec_params("INSERT INTO retired_players (name, score, play_time_seconds) VALUES ($1, $2, $3)",
                  name, score, play_time_seconds);
    w.commit();
    std::cout << "Added retired player: " << name << " (score: " << score << ")" << std::endl;
}

std::vector<RetiredPlayer> DatabaseManager::GetRecords(int start, int max_items) {
    pqxx::read_transaction r(*conn_);
    int limit = std::min(max_items, 100);
    auto result = r.exec_params("SELECT name, score, play_time_seconds FROM retired_players "
                                "ORDER BY score DESC, play_time_seconds ASC, name ASC "
                                "OFFSET $1 LIMIT $2", start, limit);
    std::vector<RetiredPlayer> records;
    for (const auto& row : result) {
        records.push_back({row[0].as<std::string>(), row[1].as<int>(), row[2].as<double>()});
    }
    return records;
}

} // namespace database
