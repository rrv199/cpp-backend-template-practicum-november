#include "database.hpp"
#include <iostream>
#include <sstream>

namespace database {

DatabaseManager::DatabaseManager(const std::string& connection_string) {
    try {
        conn_ = std::make_shared<pqxx::connection>(connection_string);
        if (!conn_->is_open()) {
            throw std::runtime_error("Failed to connect to database");
        }
        std::cout << "Connected to database successfully" << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "Database connection error: " << e.what() << std::endl;
        throw;
    }
}

void DatabaseManager::InitializeTables() {
    pqxx::work w(*conn_);
    
    w.exec(
        "CREATE TABLE IF NOT EXISTS retired_players ("
        "id SERIAL PRIMARY KEY, "
        "name VARCHAR(100) NOT NULL, "
        "score INTEGER NOT NULL DEFAULT 0, "
        "play_time_seconds DOUBLE PRECISION NOT NULL"
        ");"
    );
    
    w.exec(
        "CREATE INDEX IF NOT EXISTS idx_retired_players_score_time_name "
        "ON retired_players (score DESC, play_time_seconds ASC, name ASC);"
    );
    
    w.commit();
    std::cout << "Database tables initialized" << std::endl;
}

void DatabaseManager::AddRetiredPlayer(const std::string& name, int score, double play_time_seconds) {
    pqxx::work w(*conn_);
    w.exec_params(
        "INSERT INTO retired_players (name, score, play_time_seconds) VALUES ($1, $2, $3)",
        name, score, play_time_seconds
    );
    w.commit();
    std::cout << "Added retired player: " << name << " (score: " << score 
              << ", playTime: " << play_time_seconds << "s)" << std::endl;
}

std::vector<RetiredPlayer> DatabaseManager::GetRecords(int start, int max_items) {
    pqxx::read_transaction r(*conn_);
    
    int limit = std::min(max_items, 100);
    
    auto result = r.exec_params(
        "SELECT name, score, play_time_seconds FROM retired_players "
        "ORDER BY score DESC, play_time_seconds ASC, name ASC "
        "OFFSET $1 LIMIT $2",
        start, limit
    );
    
    std::vector<RetiredPlayer> records;
    for (const auto& row : result) {
        records.push_back(RetiredPlayer{
            .name = row[0].as<std::string>(),
            .score = row[1].as<int>(),
            .play_time_seconds = row[2].as<double>()
        });
    }
    return records;
}

int DatabaseManager::GetTotalRecordsCount() {
    pqxx::read_transaction r(*conn_);
    auto result = r.exec("SELECT COUNT(*) FROM retired_players");
    if (!result.empty()) {
        return result[0][0].as<int>();
    }
    return 0;
}

} // namespace database
