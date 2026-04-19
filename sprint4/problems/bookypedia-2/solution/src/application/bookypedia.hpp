#pragma once

#include "../infrastructure/unit_of_work.hpp"
#include "../../include/domain/models.hpp"
#include <memory>
#include <string>
#include <iostream>
#include <vector>

namespace application {

class Bookypedia {
public:
    explicit Bookypedia(std::unique_ptr<infrastructure::UnitOfWorkFactory> uow_factory);
    
    void AddAuthor(const std::string& name);
    void ShowAuthors();
    void AddBook(int year, const std::string& title, std::istream& in, std::ostream& out);
    void ShowAuthorBooks(std::istream& in, std::ostream& out);
    void ShowBooks();
    void DeleteAuthor(const std::string& name, std::istream& in, std::ostream& out);
    void EditAuthor(const std::string& name, std::istream& in, std::ostream& out);
    void ShowBook(const std::string& title, std::istream& in, std::ostream& out);
    void DeleteBook(const std::string& title, std::istream& in, std::ostream& out);
    void EditBook(const std::string& title, std::istream& in, std::ostream& out);
    
private:
    std::vector<std::string> NormalizeTags(const std::string& tags_input);
    void PrintBookWithTags(const domain::Book& book, domain::AuthorRepository& authors,
                          domain::TagRepository& tags, std::ostream& out, int index = -1);
    
    std::unique_ptr<infrastructure::UnitOfWorkFactory> uow_factory_;
};

} // namespace application
