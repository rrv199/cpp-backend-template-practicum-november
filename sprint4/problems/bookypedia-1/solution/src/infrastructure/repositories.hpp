#pragma once

#include "../../include/domain/repositories.hpp"
#include <pqxx/pqxx>
#include <memory>

namespace infrastructure {

class PostgresAuthorRepository : public domain::AuthorRepository {
public:
    explicit PostgresAuthorRepository(std::shared_ptr<pqxx::connection> conn);
    
    void Add(const domain::Author& author) override;
    std::optional<domain::Author> FindByName(const std::string& name) override;
    std::vector<domain::Author> GetAllSortedByName() override;
    bool Exists(const std::string& name) override;
    
private:
    std::shared_ptr<pqxx::connection> conn_;
};

class PostgresBookRepository : public domain::BookRepository {
public:
    explicit PostgresBookRepository(std::shared_ptr<pqxx::connection> conn);
    
    void Add(const domain::Book& book) override;
    std::vector<domain::Book> GetByAuthorId(const domain::Id& author_id) override;
    std::vector<domain::Book> GetAllSortedByTitle() override;
    
private:
    std::shared_ptr<pqxx::connection> conn_;
};

} // namespace infrastructure
