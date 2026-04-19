#include "repositories.hpp"
#include <boost/lexical_cast.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <boost/uuid/uuid_generators.hpp>

namespace infrastructure {

namespace {

std::string UuidToString(const domain::Id& uuid) {
    return boost::lexical_cast<std::string>(uuid);
}

domain::Id StringToUuid(const std::string& str) {
    boost::uuids::string_generator gen;
    return gen(str);
}

} // anonymous namespace

// AuthorRepository implementation
PostgresAuthorRepository::PostgresAuthorRepository(pqxx::work& transaction)
    : transaction_(transaction) {}

void PostgresAuthorRepository::Add(const domain::Author& author) {
    transaction_.exec_params(
        "INSERT INTO authors (id, name) VALUES ($1, $2)",
        UuidToString(author.id), author.name
    );
}

std::optional<domain::Author> PostgresAuthorRepository::FindById(const domain::Id& id) {
    auto result = transaction_.exec_params(
        "SELECT name FROM authors WHERE id = $1",
        UuidToString(id)
    );
    
    if (!result.empty()) {
        return domain::Author{
            .id = id,
            .name = result[0][0].as<std::string>()
        };
    }
    return std::nullopt;
}

std::optional<domain::Author> PostgresAuthorRepository::FindByName(const std::string& name) {
    auto result = transaction_.exec_params(
        "SELECT id FROM authors WHERE name = $1",
        name
    );
    
    if (!result.empty()) {
        return domain::Author{
            .id = StringToUuid(result[0][0].as<std::string>()),
            .name = name
        };
    }
    return std::nullopt;
}

std::vector<domain::Author> PostgresAuthorRepository::GetAllSortedByName() {
    auto result = transaction_.exec("SELECT id, name FROM authors ORDER BY name");
    
    std::vector<domain::Author> authors;
    for (const auto& row : result) {
        authors.push_back(domain::Author{
            .id = StringToUuid(row[0].as<std::string>()),
            .name = row[1].as<std::string>()
        });
    }
    return authors;
}

bool PostgresAuthorRepository::Exists(const std::string& name) {
    auto result = transaction_.exec_params(
        "SELECT 1 FROM authors WHERE name = $1 LIMIT 1", name
    );
    return !result.empty();
}

void PostgresAuthorRepository::Delete(const domain::Id& id) {
    transaction_.exec_params(
        "DELETE FROM authors WHERE id = $1", UuidToString(id)
    );
}

void PostgresAuthorRepository::Update(const domain::Author& author) {
    transaction_.exec_params(
        "UPDATE authors SET name = $1 WHERE id = $2",
        author.name, UuidToString(author.id)
    );
}

// BookRepository implementation
PostgresBookRepository::PostgresBookRepository(pqxx::work& transaction)
    : transaction_(transaction) {}

void PostgresBookRepository::Add(const domain::Book& book) {
    transaction_.exec_params(
        "INSERT INTO books (id, author_id, title, publication_year) VALUES ($1, $2, $3, $4)",
        UuidToString(book.id), UuidToString(book.author_id), book.title, book.publication_year
    );
}

std::optional<domain::Book> PostgresBookRepository::FindById(const domain::Id& id) {
    auto result = transaction_.exec_params(
        "SELECT author_id, title, publication_year FROM books WHERE id = $1",
        UuidToString(id)
    );
    
    if (!result.empty()) {
        return domain::Book{
            .id = id,
            .author_id = StringToUuid(result[0][0].as<std::string>()),
            .title = result[0][1].as<std::string>(),
            .publication_year = result[0][2].as<int>()
        };
    }
    return std::nullopt;
}

std::vector<domain::Book> PostgresBookRepository::FindByTitle(const std::string& title) {
    auto result = transaction_.exec_params(
        "SELECT id, author_id, publication_year FROM books WHERE title = $1",
        title
    );
    
    std::vector<domain::Book> books;
    for (const auto& row : result) {
        books.push_back(domain::Book{
            .id = StringToUuid(row[0].as<std::string>()),
            .author_id = StringToUuid(row[1].as<std::string>()),
            .title = title,
            .publication_year = row[2].as<int>()
        });
    }
    return books;
}

std::vector<domain::Book> PostgresBookRepository::GetByAuthorId(const domain::Id& author_id) {
    auto result = transaction_.exec_params(
        "SELECT id, title, publication_year FROM books WHERE author_id = $1",
        UuidToString(author_id)
    );
    
    std::vector<domain::Book> books;
    for (const auto& row : result) {
        books.push_back(domain::Book{
            .id = StringToUuid(row[0].as<std::string>()),
            .author_id = author_id,
            .title = row[1].as<std::string>(),
            .publication_year = row[2].as<int>()
        });
    }
    return books;
}

std::vector<domain::Book> PostgresBookRepository::GetAllSorted() {
    auto result = transaction_.exec(
        "SELECT b.id, b.title, b.publication_year, a.name FROM books b "
        "JOIN authors a ON b.author_id = a.id "
        "ORDER BY b.title, a.name, b.publication_year"
    );
    
    std::vector<domain::Book> books;
    for (const auto& row : result) {
        books.push_back(domain::Book{
            .id = StringToUuid(row[0].as<std::string>()),
            .author_id = StringToUuid(row[1].as<std::string>()),
            .title = row[2].as<std::string>(),
            .publication_year = row[3].as<int>()
        });
    }
    return books;
}

void PostgresBookRepository::Delete(const domain::Id& id) {
    transaction_.exec_params("DELETE FROM books WHERE id = $1", UuidToString(id));
}

void PostgresBookRepository::DeleteByAuthorId(const domain::Id& author_id) {
    transaction_.exec_params(
        "DELETE FROM books WHERE author_id = $1", UuidToString(author_id)
    );
}

void PostgresBookRepository::Update(const domain::Book& book) {
    transaction_.exec_params(
        "UPDATE books SET title = $1, publication_year = $2, author_id = $3 WHERE id = $4",
        book.title, book.publication_year, UuidToString(book.author_id), UuidToString(book.id)
    );
}

// TagRepository implementation
PostgresTagRepository::PostgresTagRepository(pqxx::work& transaction)
    : transaction_(transaction) {}

void PostgresTagRepository::AddTags(const std::vector<domain::BookTag>& tags) {
    for (const auto& tag : tags) {
        transaction_.exec_params(
            "INSERT INTO book_tags (book_id, tag) VALUES ($1, $2)",
            UuidToString(tag.book_id), tag.tag
        );
    }
}

std::vector<std::string> PostgresTagRepository::GetTagsByBookId(const domain::Id& book_id) {
    auto result = transaction_.exec_params(
        "SELECT tag FROM book_tags WHERE book_id = $1 ORDER BY tag",
        UuidToString(book_id)
    );
    
    std::vector<std::string> tags;
    for (const auto& row : result) {
        tags.push_back(row[0].as<std::string>());
    }
    return tags;
}

void PostgresTagRepository::DeleteByBookId(const domain::Id& book_id) {
    transaction_.exec_params(
        "DELETE FROM book_tags WHERE book_id = $1", UuidToString(book_id)
    );
}

void PostgresTagRepository::UpdateTags(const domain::Id& book_id, const std::vector<std::string>& tags) {
    DeleteByBookId(book_id);
    for (const auto& tag : tags) {
        transaction_.exec_params(
            "INSERT INTO book_tags (book_id, tag) VALUES ($1, $2)",
            UuidToString(book_id), tag
        );
    }
}

} // namespace infrastructure
