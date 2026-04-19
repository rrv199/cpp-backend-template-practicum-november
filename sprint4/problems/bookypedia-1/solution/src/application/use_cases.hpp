#pragma once

#include "../../include/domain/repositories.hpp"
#include <memory>
#include <string>
#include <iostream>

namespace application {

class Bookypedia {
public:
    Bookypedia(
        std::unique_ptr<domain::AuthorRepository> author_repo,
        std::unique_ptr<domain::BookRepository> book_repo
    );
    
    void AddAuthor(const std::string& name);
    void ShowAuthors();
    void AddBook(int year, const std::string& title, std::istream& in, std::ostream& out);
    void ShowAuthorBooks(std::istream& in, std::ostream& out);
    void ShowBooks();
    
private:
    std::unique_ptr<domain::AuthorRepository> author_repo_;
    std::unique_ptr<domain::BookRepository> book_repo_;
};

} // namespace application
