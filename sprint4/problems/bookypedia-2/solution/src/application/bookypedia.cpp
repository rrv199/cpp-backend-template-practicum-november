#include "bookypedia.hpp"
#include <boost/uuid/uuid_generators.hpp>
#include <algorithm>
#include <sstream>
#include <set>
#include <cctype>

namespace application {

Bookypedia::Bookypedia(std::unique_ptr<infrastructure::UnitOfWorkFactory> uow_factory)
    : uow_factory_(std::move(uow_factory)) {}

std::vector<std::string> Bookypedia::NormalizeTags(const std::string& tags_input) {
    std::vector<std::string> result;
    std::set<std::string> unique;
    
    std::stringstream ss(tags_input);
    std::string tag;
    
    while (std::getline(ss, tag, ',')) {
        size_t start = tag.find_first_not_of(" \t");
        size_t end = tag.find_last_not_of(" \t");
        
        if (start == std::string::npos) continue;
        
        tag = tag.substr(start, end - start + 1);
        
        std::string normalized;
        bool in_space = false;
        for (char c : tag) {
            if (c == ' ') {
                if (!in_space) {
                    normalized += ' ';
                    in_space = true;
                }
            } else {
                normalized += c;
                in_space = false;
            }
        }
        
        if (!normalized.empty() && unique.find(normalized) == unique.end()) {
            unique.insert(normalized);
            result.push_back(normalized);
        }
    }
    
    return result;
}

void Bookypedia::PrintBookWithTags(const domain::Book& book, domain::AuthorRepository& authors,
                                  domain::TagRepository& tags, std::ostream& out, int index) {
    auto author_opt = authors.FindById(book.author_id);
    if (!author_opt) return;
    
    if (index > 0) {
        out << index << " ";
    }
    out << book.title << " by " << author_opt->name << ", " << book.publication_year;
    
    auto book_tags = tags.GetTagsByBookId(book.id);
    if (!book_tags.empty()) {
        out << std::endl << "Tags: ";
        for (size_t i = 0; i < book_tags.size(); ++i) {
            if (i > 0) out << ", ";
            out << book_tags[i];
        }
    }
    out << std::endl;
}

void Bookypedia::AddAuthor(const std::string& name) {
    auto uow = uow_factory_->Create();
    auto& authors = uow->Authors();
    
    std::string trimmed = name;
    trimmed.erase(0, trimmed.find_first_not_of(" \t"));
    trimmed.erase(trimmed.find_last_not_of(" \t") + 1);
    
    if (trimmed.empty() || authors.Exists(trimmed)) {
        std::cout << "Failed to add author" << std::endl;
        uow->Rollback();
        return;
    }
    
    domain::Author author{
        .id = boost::uuids::random_generator()(),
        .name = trimmed
    };
    
    authors.Add(author);
    uow->Commit();
}

void Bookypedia::ShowAuthors() {
    auto uow = uow_factory_->Create();
    auto authors = uow->Authors().GetAllSortedByName();
    
    for (size_t i = 0; i < authors.size(); ++i) {
        std::cout << i + 1 << ". " << authors[i].name << std::endl;
    }
    uow->Commit();
}

void Bookypedia::AddBook(int year, const std::string& title, std::istream& in, std::ostream& out) {
    auto uow = uow_factory_->Create();
    auto& authors = uow->Authors();
    auto& books = uow->Books();
    auto& tags_repo = uow->Tags();
    
    out << "Enter author name or empty line to select from list:" << std::endl;
    std::string author_input;
    std::getline(in, author_input);
    
    domain::Id author_id;
    bool author_found = false;
    
    // Удаляем пробелы
    author_input.erase(0, author_input.find_first_not_of(" \t"));
    author_input.erase(author_input.find_last_not_of(" \t") + 1);
    
    if (author_input.empty()) {
        // Выбор из списка
        auto all_authors = authors.GetAllSortedByName();
        if (all_authors.empty()) {
            out << "No authors in database. Please add author first." << std::endl;
            uow->Rollback();
            return;
        }
        
        out << "Select author:" << std::endl;
        for (size_t i = 0; i < all_authors.size(); ++i) {
            out << i + 1 << ". " << all_authors[i].name << std::endl;
        }
        out << "Enter author # or empty line to cancel:" << std::endl;
        
        std::string line;
        std::getline(in, line);
        
        if (line.empty()) {
            uow->Rollback();
            return;
        }
        
        // Проверяем, является ли ввод числом
        bool is_number = !line.empty() && std::all_of(line.begin(), line.end(), ::isdigit);
        
        if (is_number) {
            try {
                size_t idx = std::stoul(line) - 1;
                if (idx < all_authors.size()) {
                    author_id = all_authors[idx].id;
                    author_found = true;
                }
            } catch (...) {
                // Не число, возможно имя
            }
        }
        
        if (!author_found) {
            // Пробуем найти по имени
            auto existing = authors.FindByName(line);
            if (existing) {
                author_id = existing->id;
                author_found = true;
            }
        }
    } else {
        // Поиск по имени
        auto existing = authors.FindByName(author_input);
        if (existing) {
            author_id = existing->id;
            author_found = true;
        } else {
            out << "No author found. Do you want to add " << author_input << " (y/n)?" << std::endl;
            std::string answer;
            std::getline(in, answer);
            if (answer == "y" || answer == "Y") {
                domain::Author new_author{
                    .id = boost::uuids::random_generator()(),
                    .name = author_input
                };
                authors.Add(new_author);
                author_id = new_author.id;
                author_found = true;
            }
        }
    }
    
    if (!author_found) {
        out << "Failed to add book" << std::endl;
        uow->Rollback();
        return;
    }
    
    out << "Enter tags (comma separated):" << std::endl;
    std::string tags_line;
    std::getline(in, tags_line);
    
    auto tags = NormalizeTags(tags_line);
    
    domain::Book book{
        .id = boost::uuids::random_generator()(),
        .author_id = author_id,
        .title = title,
        .publication_year = year
    };
    
    books.Add(book);
    
    for (const auto& tag : tags) {
        tags_repo.AddTags({{book.id, tag}});
    }
    
    uow->Commit();
}

void Bookypedia::ShowAuthorBooks(std::istream& in, std::ostream& out) {
    auto uow = uow_factory_->Create();
    auto& authors = uow->Authors();
    auto& books = uow->Books();
    
    auto all_authors = authors.GetAllSortedByName();
    
    if (all_authors.empty()) {
        uow->Rollback();
        return;
    }
    
    out << "Select author:" << std::endl;
    for (size_t i = 0; i < all_authors.size(); ++i) {
        out << i + 1 << ". " << all_authors[i].name << std::endl;
    }
    out << "Enter author # or empty line to cancel:" << std::endl;
    
    std::string line;
    std::getline(in, line);
    
    if (line.empty()) {
        uow->Rollback();
        return;
    }
    
    try {
        size_t idx = std::stoul(line) - 1;
        if (idx >= all_authors.size()) {
            uow->Rollback();
            return;
        }
        
        const auto& author = all_authors[idx];
        auto author_books = books.GetByAuthorId(author.id);
        
        for (size_t i = 0; i < author_books.size(); ++i) {
            out << i + 1 << " " << author_books[i].title << ", " << author_books[i].publication_year << std::endl;
        }
        
    } catch (...) {
        uow->Rollback();
    }
    
    uow->Commit();
}

void Bookypedia::ShowBooks() {
    auto uow = uow_factory_->Create();
    auto& authors = uow->Authors();
    auto& books = uow->Books();
    
    auto all_books = books.GetAllSorted();
    
    for (size_t i = 0; i < all_books.size(); ++i) {
        const auto& book = all_books[i];
        auto author_opt = authors.FindById(book.author_id);
        if (author_opt) {
            std::cout << i + 1 << ". " << book.title << " by " << author_opt->name
                      << ", " << book.publication_year << std::endl;
        }
    }
    
    uow->Commit();
}

void Bookypedia::DeleteAuthor(const std::string& name, std::istream& in, std::ostream& out) {
    auto uow = uow_factory_->Create();
    auto& authors = uow->Authors();
    
    std::string author_name = name;
    domain::Id author_id;
    bool found = false;
    
    if (author_name.empty()) {
        auto all_authors = authors.GetAllSortedByName();
        if (all_authors.empty()) {
            out << "Failed to delete author" << std::endl;
            uow->Rollback();
            return;
        }
        
        out << "Select author:" << std::endl;
        for (size_t i = 0; i < all_authors.size(); ++i) {
            out << i + 1 << ". " << all_authors[i].name << std::endl;
        }
        out << "Enter author # or empty line to cancel:" << std::endl;
        
        std::string line;
        std::getline(in, line);
        if (line.empty()) {
            uow->Rollback();
            return;
        }
        
        try {
            size_t idx = std::stoul(line) - 1;
            if (idx < all_authors.size()) {
                author_id = all_authors[idx].id;
                found = true;
            }
        } catch (...) {
            // Not a number
        }
        
        if (!found) {
            auto author_opt = authors.FindByName(line);
            if (author_opt) {
                author_id = author_opt->id;
                found = true;
            }
        }
    } else {
        auto author_opt = authors.FindByName(author_name);
        if (author_opt) {
            author_id = author_opt->id;
            found = true;
        }
    }
    
    if (!found) {
        out << "Failed to delete author" << std::endl;
        uow->Rollback();
        return;
    }
    
    authors.Delete(author_id);
    uow->Commit();
}

void Bookypedia::EditAuthor(const std::string& name, std::istream& in, std::ostream& out) {
    auto uow = uow_factory_->Create();
    auto& authors = uow->Authors();
    
    std::string author_name = name;
    domain::Author author;
    bool found = false;
    
    if (author_name.empty()) {
        auto all_authors = authors.GetAllSortedByName();
        if (all_authors.empty()) {
            out << "Failed to edit author" << std::endl;
            uow->Rollback();
            return;
        }
        
        out << "Select author:" << std::endl;
        for (size_t i = 0; i < all_authors.size(); ++i) {
            out << i + 1 << ". " << all_authors[i].name << std::endl;
        }
        out << "Enter author # or empty line to cancel:" << std::endl;
        
        std::string line;
        std::getline(in, line);
        if (line.empty()) {
            uow->Rollback();
            return;
        }
        
        try {
            size_t idx = std::stoul(line) - 1;
            if (idx < all_authors.size()) {
                author = all_authors[idx];
                found = true;
            }
        } catch (...) {
            auto author_opt = authors.FindByName(line);
            if (author_opt) {
                author = *author_opt;
                found = true;
            }
        }
    } else {
        auto author_opt = authors.FindByName(author_name);
        if (author_opt) {
            author = *author_opt;
            found = true;
        }
    }
    
    if (!found) {
        out << "Failed to edit author" << std::endl;
        uow->Rollback();
        return;
    }
    
    out << "Enter new name:" << std::endl;
    std::string new_name;
    std::getline(in, new_name);
    
    if (new_name.empty()) {
        out << "Failed to edit author" << std::endl;
        uow->Rollback();
        return;
    }
    
    author.name = new_name;
    authors.Update(author);
    uow->Commit();
}

void Bookypedia::ShowBook(const std::string& title, std::istream& in, std::ostream& out) {
    auto uow = uow_factory_->Create();
    auto& authors = uow->Authors();
    auto& books_repo = uow->Books();
    auto& tags_repo = uow->Tags();
    
    std::string book_title = title;
    std::vector<domain::Book> books;
    
    if (book_title.empty()) {
        books = books_repo.GetAllSorted();
        if (books.empty()) {
            uow->Rollback();
            return;
        }
        
        out << "Select book:" << std::endl;
        for (size_t i = 0; i < books.size(); ++i) {
            auto author_opt = authors.FindById(books[i].author_id);
            if (author_opt) {
                out << i + 1 << ". " << books[i].title << " by " << author_opt->name
                    << ", " << books[i].publication_year << std::endl;
            }
        }
        out << "Enter the book # or empty line to cancel:" << std::endl;
        
        std::string line;
        std::getline(in, line);
        if (line.empty()) {
            uow->Rollback();
            return;
        }
        
        try {
            size_t idx = std::stoul(line) - 1;
            if (idx >= books.size()) {
                uow->Rollback();
                return;
            }
            PrintBookWithTags(books[idx], authors, tags_repo, out);
        } catch (...) {
            uow->Rollback();
            return;
        }
    } else {
        books = books_repo.FindByTitle(book_title);
        if (books.empty()) {
            uow->Rollback();
            return;
        }
        
        if (books.size() == 1) {
            PrintBookWithTags(books[0], authors, tags_repo, out);
        } else {
            out << "Select book:" << std::endl;
            for (size_t i = 0; i < books.size(); ++i) {
                auto author_opt = authors.FindById(books[i].author_id);
                if (author_opt) {
                    out << i + 1 << ". " << books[i].title << " by " << author_opt->name
                        << ", " << books[i].publication_year << std::endl;
                }
            }
            out << "Enter the book # or empty line to cancel:" << std::endl;
            
            std::string line;
            std::getline(in, line);
            if (line.empty()) {
                uow->Rollback();
                return;
            }
            
            try {
                size_t idx = std::stoul(line) - 1;
                if (idx >= books.size()) {
                    uow->Rollback();
                    return;
                }
                PrintBookWithTags(books[idx], authors, tags_repo, out);
            } catch (...) {
                uow->Rollback();
                return;
            }
        }
    }
    
    uow->Commit();
}

void Bookypedia::DeleteBook(const std::string& title, std::istream& in, std::ostream& out) {
    auto uow = uow_factory_->Create();
    auto& authors = uow->Authors();
    auto& books_repo = uow->Books();
    
    std::string book_title = title;
    std::vector<domain::Book> books;
    
    if (book_title.empty()) {
        books = books_repo.GetAllSorted();
        if (books.empty()) {
            out << "Failed to delete book" << std::endl;
            uow->Rollback();
            return;
        }
        
        out << "Select book to delete:" << std::endl;
        for (size_t i = 0; i < books.size(); ++i) {
            auto author_opt = authors.FindById(books[i].author_id);
            if (author_opt) {
                out << i + 1 << ". " << books[i].title << " by " << author_opt->name
                    << ", " << books[i].publication_year << std::endl;
            }
        }
        out << "Enter the book # or empty line to cancel:" << std::endl;
        
        std::string line;
        std::getline(in, line);
        if (line.empty()) {
            out << "Failed to delete book" << std::endl;
            uow->Rollback();
            return;
        }
        
        try {
            size_t idx = std::stoul(line) - 1;
            if (idx >= books.size()) {
                out << "Failed to delete book" << std::endl;
                uow->Rollback();
                return;
            }
            books_repo.Delete(books[idx].id);
        } catch (...) {
            out << "Failed to delete book" << std::endl;
            uow->Rollback();
            return;
        }
    } else {
        books = books_repo.FindByTitle(book_title);
        if (books.empty()) {
            out << "Failed to delete book" << std::endl;
            uow->Rollback();
            return;
        }
        
        if (books.size() == 1) {
            books_repo.Delete(books[0].id);
        } else {
            out << "Multiple books found. Select book to delete:" << std::endl;
            for (size_t i = 0; i < books.size(); ++i) {
                auto author_opt = authors.FindById(books[i].author_id);
                if (author_opt) {
                    out << i + 1 << ". " << books[i].title << " by " << author_opt->name
                        << ", " << books[i].publication_year << std::endl;
                }
            }
            out << "Enter the book # or empty line to cancel:" << std::endl;
            
            std::string line;
            std::getline(in, line);
            if (line.empty()) {
                out << "Failed to delete book" << std::endl;
                uow->Rollback();
                return;
            }
            
            try {
                size_t idx = std::stoul(line) - 1;
                if (idx >= books.size()) {
                    out << "Failed to delete book" << std::endl;
                    uow->Rollback();
                    return;
                }
                books_repo.Delete(books[idx].id);
            } catch (...) {
                out << "Failed to delete book" << std::endl;
                uow->Rollback();
                return;
            }
        }
    }
    
    uow->Commit();
}

void Bookypedia::EditBook(const std::string& title, std::istream& in, std::ostream& out) {
    auto uow = uow_factory_->Create();
    auto& authors = uow->Authors();
    auto& books_repo = uow->Books();
    auto& tags_repo = uow->Tags();
    
    std::string book_title = title;
    std::vector<domain::Book> books;
    
    if (book_title.empty()) {
        books = books_repo.GetAllSorted();
        if (books.empty()) {
            out << "Book not found" << std::endl;
            uow->Rollback();
            return;
        }
        
        out << "Select book to edit:" << std::endl;
        for (size_t i = 0; i < books.size(); ++i) {
            auto author_opt = authors.FindById(books[i].author_id);
            if (author_opt) {
                out << i + 1 << ". " << books[i].title << " by " << author_opt->name
                    << ", " << books[i].publication_year << std::endl;
            }
        }
        out << "Enter the book # or empty line to cancel:" << std::endl;
        
        std::string line;
        std::getline(in, line);
        if (line.empty()) {
            uow->Rollback();
            return;
        }
        
        try {
            size_t idx = std::stoul(line) - 1;
            if (idx >= books.size()) {
                out << "Book not found" << std::endl;
                uow->Rollback();
                return;
            }
            books = {books[idx]};
        } catch (...) {
            out << "Book not found" << std::endl;
            uow->Rollback();
            return;
        }
    } else {
        books = books_repo.FindByTitle(book_title);
        if (books.empty()) {
            out << "Book not found" << std::endl;
            uow->Rollback();
            return;
        }
        
        if (books.size() > 1) {
            out << "Multiple books found. Select book to edit:" << std::endl;
            for (size_t i = 0; i < books.size(); ++i) {
                auto author_opt = authors.FindById(books[i].author_id);
                if (author_opt) {
                    out << i + 1 << ". " << books[i].title << " by " << author_opt->name
                        << ", " << books[i].publication_year << std::endl;
                }
            }
            out << "Enter the book # or empty line to cancel:" << std::endl;
            
            std::string line;
            std::getline(in, line);
            if (line.empty()) {
                uow->Rollback();
                return;
            }
            
            try {
                size_t idx = std::stoul(line) - 1;
                if (idx >= books.size()) {
                    out << "Book not found" << std::endl;
                    uow->Rollback();
                    return;
                }
                books = {books[idx]};
            } catch (...) {
                out << "Book not found" << std::endl;
                uow->Rollback();
                return;
            }
        }
    }
    
    if (books.empty() || books.size() != 1) {
        out << "Book not found" << std::endl;
        uow->Rollback();
        return;
    }
    
    auto& book = books[0];
    auto current_tags = tags_repo.GetTagsByBookId(book.id);
    
    out << "Enter new title or empty line to use the current one (" << book.title << "):" << std::endl;
    std::string new_title;
    std::getline(in, new_title);
    if (!new_title.empty()) {
        book.title = new_title;
    }
    
    out << "Enter publication year or empty line to use the current one (" << book.publication_year << "):" << std::endl;
    std::string year_str;
    std::getline(in, year_str);
    if (!year_str.empty()) {
        try {
            book.publication_year = std::stoi(year_str);
        } catch (...) {}
    }
    
    std::string tags_str;
    if (!current_tags.empty()) {
        out << "Enter tags (current tags: ";
        for (size_t i = 0; i < current_tags.size(); ++i) {
            if (i > 0) out << ", ";
            out << current_tags[i];
        }
        out << "):" << std::endl;
    } else {
        out << "Enter tags (current tags: none):" << std::endl;
    }
    std::getline(in, tags_str);
    
    if (!tags_str.empty()) {
        auto new_tags = NormalizeTags(tags_str);
        tags_repo.UpdateTags(book.id, new_tags);
    }
    
    books_repo.Update(book);
    uow->Commit();
}

} // namespace application
