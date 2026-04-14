#pragma once

#include "repositories.hpp"
#include <pqxx/pqxx>
#include <memory>

namespace infrastructure {

class UnitOfWork {
public:
    explicit UnitOfWork(pqxx::connection& conn);
    
    domain::AuthorRepository& Authors() { return author_repo_; }
    domain::BookRepository& Books() { return book_repo_; }
    domain::TagRepository& Tags() { return tag_repo_; }
    
    void Commit();
    void Rollback();
    
private:
    pqxx::work transaction_;
    PostgresAuthorRepository author_repo_;
    PostgresBookRepository book_repo_;
    PostgresTagRepository tag_repo_;
};

class UnitOfWorkFactory {
public:
    explicit UnitOfWorkFactory(std::shared_ptr<pqxx::connection> conn);
    
    std::unique_ptr<UnitOfWork> Create();
    
private:
    std::shared_ptr<pqxx::connection> conn_;
};

} // namespace infrastructure
