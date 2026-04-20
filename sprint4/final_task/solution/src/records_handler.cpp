#include "records_handler.hpp"
#include <nlohmann/json.hpp>

namespace http {

constexpr int MAX_ITEMS_LIMIT = 100;

RecordsHandler::RecordsHandler(database::DatabaseManager& db_manager) : db_manager_(db_manager) {}

bool RecordsHandler::ValidateParams(int start, int max_items, std::string& error_message) {
    if (start < 0) {
        error_message = "start must be >= 0";
        return false;
    }
    if (max_items < 1) {
        error_message = "maxItems must be >= 1";
        return false;
    }
    if (max_items > MAX_ITEMS_LIMIT) {
        error_message = "maxItems must be <= " + std::to_string(MAX_ITEMS_LIMIT);
        return false;
    }
    return true;
}

std::string RecordsHandler::HandleGetRecords(int start, int max_items) {
    auto records = db_manager_.GetRecords(start, max_items);
    
    nlohmann::json json_array = nlohmann::json::array();
    for (const auto& record : records) {
        json_array.push_back({
            {"name", record.name},
            {"score", record.score},
            {"playTime", record.play_time_seconds}
        });
    }
    return json_array.dump();
}

} // namespace http
