#pragma once

#include "models.hpp"
#include <optional>
#include <vector>

namespace domain {

class AuthorRepository {
public:
    virtual ~AuthorRepository() = default;
    
    virtual void Add(const Author& author) = 0;
    virtual std::optional<Author> FindByName(const std::string& name) = 0;
    virtual std::vector<Author> GetAllSortedByName() = 0;
    virtual bool Exists(const std::string& name) = 0;
};

class BookRepository {
public:
    virtual ~BookRepository() = default;
    
    virtual void Add(const Book& book) = 0;
    virtual std::vector<Book> GetByAuthorId(const Id& author_id) = 0;
    virtual std::vector<Book> GetAllSortedByTitle() = 0;
};

} // namespace domain
