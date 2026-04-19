#pragma once

#include "models.hpp"
#include <optional>
#include <vector>

namespace domain {

class AuthorRepository {
public:
    virtual ~AuthorRepository() = default;
    
    virtual void Add(const Author& author) = 0;
    virtual std::optional<Author> FindById(const Id& id) = 0;
    virtual std::optional<Author> FindByName(const std::string& name) = 0;
    virtual std::vector<Author> GetAllSortedByName() = 0;
    virtual bool Exists(const std::string& name) = 0;
    virtual void Delete(const Id& id) = 0;
    virtual void Update(const Author& author) = 0;
};

class BookRepository {
public:
    virtual ~BookRepository() = default;
    
    virtual void Add(const Book& book) = 0;
    virtual std::optional<Book> FindById(const Id& id) = 0;
    virtual std::vector<Book> FindByTitle(const std::string& title) = 0;
    virtual std::vector<Book> GetByAuthorId(const Id& author_id) = 0;
    virtual std::vector<Book> GetAllSorted() = 0;
    virtual void Delete(const Id& id) = 0;
    virtual void DeleteByAuthorId(const Id& author_id) = 0;
    virtual void Update(const Book& book) = 0;
};

class TagRepository {
public:
    virtual ~TagRepository() = default;
    
    virtual void AddTags(const std::vector<BookTag>& tags) = 0;
    virtual std::vector<std::string> GetTagsByBookId(const Id& book_id) = 0;
    virtual void DeleteByBookId(const Id& book_id) = 0;
    virtual void UpdateTags(const Id& book_id, const std::vector<std::string>& tags) = 0;
};

} // namespace domain
