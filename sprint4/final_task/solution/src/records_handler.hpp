#pragma once
#include "database.hpp"
#include <string>

namespace http {

class RecordsHandler {
public:
    explicit RecordsHandler(database::DatabaseManager& db_manager);
    std::string HandleGetRecords(int start, int max_items);
    bool ValidateParams(int start, int max_items, std::string& error_message);
    
private:
    database::DatabaseManager& db_manager_;
};

}
