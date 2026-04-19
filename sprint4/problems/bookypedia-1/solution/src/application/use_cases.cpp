#include "use_cases.hpp"
#include <boost/uuid/uuid_generators.hpp>
#include <algorithm>
#include <iostream>

namespace application {

Bookypedia::Bookypedia(
    std::unique_ptr<domain::AuthorRepository> author_repo,
    std::unique_ptr<domain::BookRepository> book_repo
) : author_repo_(std::move(author_repo))
    , book_repo_(std::move(book_repo)) {
}

void Bookypedia::AddAuthor(const std::string& name) {
    // Удаляем пробелы в начале и конце
    std::string trimmed = name;
    trimmed.erase(0, trimmed.find_first_not_of(" \t"));
    trimmed.erase(trimmed.find_last_not_of(" \t") + 1);
    
    if (trimmed.empty()) {
        std::cout << "Failed to add author" << std::endl;
        return;
    }
    
    // Проверяем существование автора
    if (author_repo_->Exists(trimmed)) {
        std::cout << "Failed to add author" << std::endl;
        return;
    }
    
    // Создаём автора
    domain::Author author{
        .id = boost::uuids::random_generator()(),
        .name = trimmed
    };
    
    try {
        author_repo_->Add(author);
    } catch (const std::exception&) {
        std::cout << "Failed to add author" << std::endl;
    }
}

void Bookypedia::ShowAuthors() {
    auto authors = author_repo_->GetAllSortedByName();
    for (size_t i = 0; i < authors.size(); ++i) {
        std::cout << i + 1 << ". " << authors[i].name << std::endl;
    }
}

void Bookypedia::AddBook(int year, const std::string& title, std::istream& in, std::ostream& out) {
    auto authors = author_repo_->GetAllSortedByName();
    
    if (authors.empty()) {
        return;
    }
    
    out << "Select author:" << std::endl;
    for (size_t i = 0; i < authors.size(); ++i) {
        out << i + 1 << " " << authors[i].name << std::endl;
    }
    out << "Enter author # or empty line to cancel" << std::endl;
    
    std::string line;
    std::getline(in, line);
    
    if (line.empty()) {
        return;
    }
    
    try {
        size_t idx = std::stoul(line) - 1;
        if (idx >= authors.size()) {
            return;
        }
        
        const auto& author = authors[idx];
        
        domain::Book book{
            .id = boost::uuids::random_generator()(),
            .author_id = author.id,
            .title = title,
            .publication_year = year
        };
        
        book_repo_->Add(book);
        
    } catch (const std::exception&) {
        // Игнорируем ошибки ввода
    }
}

void Bookypedia::ShowAuthorBooks(std::istream& in, std::ostream& out) {
    auto authors = author_repo_->GetAllSortedByName();
    
    if (authors.empty()) {
        return;
    }
    
    out << "Select author:" << std::endl;
    for (size_t i = 0; i < authors.size(); ++i) {
        out << i + 1 << ". " << authors[i].name << std::endl;
    }
    out << "Enter author # or empty line to cancel" << std::endl;
    
    std::string line;
    std::getline(in, line);
    
    if (line.empty()) {
        return;
    }
    
    try {
        size_t idx = std::stoul(line) - 1;
        if (idx >= authors.size()) {
            return;
        }
        
        const auto& author = authors[idx];
        auto books = book_repo_->GetByAuthorId(author.id);
        
        for (size_t i = 0; i < books.size(); ++i) {
            out << i + 1 << " " << books[i].title << ", " << books[i].publication_year << std::endl;
        }
        
    } catch (const std::exception&) {
        // Игнорируем ошибки ввода
    }
}

void Bookypedia::ShowBooks() {
    auto books = book_repo_->GetAllSortedByTitle();
    for (size_t i = 0; i < books.size(); ++i) {
        std::cout << i + 1 << ". " << books[i].title << ", " << books[i].publication_year << std::endl;
    }
}

} // namespace application
