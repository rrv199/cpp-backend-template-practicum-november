#include "records_handler.hpp"
#include <nlohmann/json.hpp>

namespace http {

RecordsHandler::RecordsHandler(database::DatabaseManager& db_manager) : db_manager_(db_manager) {}

bool RecordsHandler::ValidateParams(int start, int max_items, std::string& error_message) {
    if (start < 0) {
        error_message = "start parameter must be >= 0";
        return false;
    }
    if (max_items < 1) {
        error_message = "maxItems parameter must be >= 1";
        return false;
    }
    if (max_items > 100) {
        error_message = "maxItems parameter must be <= 100";
        return false;
    }
    return true;
}

std::string RecordsHandler::HandleGetRecords(int start, int max_items) {
    auto records = db_manager_.GetRecords(start, max_items);
    
    nlohmann::json json_array = nlohmann::json::array();
    for (const auto& record : records) {
        nlohmann::json obj;
        obj["name"] = record.name;
        obj["score"] = record.score;
        obj["playTime"] = record.play_time_seconds;
        json_array.push_back(obj);
    }
    
    return json_array.dump();
}

}
