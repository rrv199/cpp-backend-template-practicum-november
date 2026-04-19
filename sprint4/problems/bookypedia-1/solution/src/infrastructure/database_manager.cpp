#include "database_manager.hpp"
#include <iostream>

namespace infrastructure {

DatabaseManager::DatabaseManager(std::shared_ptr<pqxx::connection> conn)
    : conn_(std::move(conn)) {
}

void DatabaseManager::InitializeTables() {
    pqxx::work w(*conn_);
    
    // Таблица авторов
    w.exec(
        "CREATE TABLE IF NOT EXISTS authors ("
        "id UUID PRIMARY KEY, "
        "name VARCHAR(100) UNIQUE NOT NULL"
        ");"
    );
    
    // Таблица книг
    w.exec(
        "CREATE TABLE IF NOT EXISTS books ("
        "id UUID PRIMARY KEY, "
        "author_id UUID NOT NULL REFERENCES authors(id) ON DELETE CASCADE, "
        "title VARCHAR(100) NOT NULL, "
        "publication_year INTEGER NOT NULL"
        ");"
    );
    
    w.commit();
    std::cout << "Database tables initialized successfully" << std::endl;
}

} // namespace infrastructure
