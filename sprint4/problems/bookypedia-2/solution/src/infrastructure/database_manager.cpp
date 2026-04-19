#include "database_manager.hpp"
#include <iostream>

namespace infrastructure {

DatabaseManager::DatabaseManager(std::shared_ptr<pqxx::connection> conn)
    : conn_(std::move(conn)) {
}

void DatabaseManager::InitializeTables() {
    pqxx::work w(*conn_);
    
    w.exec(
        "CREATE TABLE IF NOT EXISTS authors ("
        "id UUID PRIMARY KEY, "
        "name VARCHAR(100) UNIQUE NOT NULL"
        ");"
    );
    
    w.exec(
        "CREATE TABLE IF NOT EXISTS books ("
        "id UUID PRIMARY KEY, "
        "author_id UUID NOT NULL REFERENCES authors(id) ON DELETE CASCADE, "
        "title VARCHAR(100) NOT NULL, "
        "publication_year INTEGER NOT NULL"
        ");"
    );
    
    w.exec(
        "CREATE TABLE IF NOT EXISTS book_tags ("
        "book_id UUID NOT NULL REFERENCES books(id) ON DELETE CASCADE, "
        "tag VARCHAR(30) NOT NULL"
        ");"
    );
    
    w.exec(
        "CREATE INDEX IF NOT EXISTS idx_book_tags_book_id ON book_tags(book_id);"
    );
    
    w.commit();
    std::cout << "Database tables initialized successfully" << std::endl;
}

} // namespace infrastructure
