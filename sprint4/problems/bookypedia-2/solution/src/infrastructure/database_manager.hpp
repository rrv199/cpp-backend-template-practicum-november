#pragma once

#include <pqxx/pqxx>
#include <memory>

namespace infrastructure {

class DatabaseManager {
public:
    explicit DatabaseManager(std::shared_ptr<pqxx::connection> conn);
    
    void InitializeTables();
    
private:
    std::shared_ptr<pqxx::connection> conn_;
};

} // namespace infrastructure
