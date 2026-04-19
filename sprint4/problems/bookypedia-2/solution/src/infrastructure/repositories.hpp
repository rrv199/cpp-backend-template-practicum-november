#pragma once

#include "../../include/domain/repositories.hpp"
#include <pqxx/pqxx>
#include <memory>

namespace infrastructure {

class PostgresAuthorRepository : public domain::AuthorRepository {
public:
    explicit PostgresAuthorRepository(pqxx::work& transaction);
    
    void Add(const domain::Author& author) override;
    std::optional<domain::Author> FindById(const domain::Id& id) override;
    std::optional<domain::Author> FindByName(const std::string& name) override;
    std::vector<domain::Author> GetAllSortedByName() override;
    bool Exists(const std::string& name) override;
    void Delete(const domain::Id& id) override;
    void Update(const domain::Author& author) override;
    
private:
    pqxx::work& transaction_;
};

class PostgresBookRepository : public domain::BookRepository {
public:
    explicit PostgresBookRepository(pqxx::work& transaction);
    
    void Add(const domain::Book& book) override;
    std::optional<domain::Book> FindById(const domain::Id& id) override;
    std::vector<domain::Book> FindByTitle(const std::string& title) override;
    std::vector<domain::Book> GetByAuthorId(const domain::Id& author_id) override;
    std::vector<domain::Book> GetAllSorted() override;
    void Delete(const domain::Id& id) override;
    void DeleteByAuthorId(const domain::Id& author_id) override;
    void Update(const domain::Book& book) override;
    
private:
    pqxx::work& transaction_;
};

class PostgresTagRepository : public domain::TagRepository {
public:
    explicit PostgresTagRepository(pqxx::work& transaction);
    
    void AddTags(const std::vector<domain::BookTag>& tags) override;
    std::vector<std::string> GetTagsByBookId(const domain::Id& book_id) override;
    void DeleteByBookId(const domain::Id& book_id) override;
    void UpdateTags(const domain::Id& book_id, const std::vector<std::string>& tags) override;
    
private:
    pqxx::work& transaction_;
};

} // namespace infrastructure
