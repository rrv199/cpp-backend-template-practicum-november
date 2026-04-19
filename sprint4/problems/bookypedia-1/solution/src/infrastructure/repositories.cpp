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

PostgresAuthorRepository::PostgresAuthorRepository(std::shared_ptr<pqxx::connection> conn)
    : conn_(std::move(conn)) {
}

void PostgresAuthorRepository::Add(const domain::Author& author) {
    pqxx::work w(*conn_);
    w.exec_params(
        "INSERT INTO authors (id, name) VALUES ($1, $2)",
        UuidToString(author.id), author.name
    );
    w.commit();
}

std::optional<domain::Author> PostgresAuthorRepository::FindByName(const std::string& name) {
    pqxx::read_transaction r(*conn_);
    auto result = r.exec_params1(
        "SELECT id FROM authors WHERE name = $1",
        name
    );
    
    if (!result.empty()) {
        return domain::Author{
            .id = StringToUuid(result[0].as<std::string>()),
            .name = name
        };
    }
    return std::nullopt;
}

std::vector<domain::Author> PostgresAuthorRepository::GetAllSortedByName() {
    pqxx::read_transaction r(*conn_);
    auto result = r.exec("SELECT id, name FROM authors ORDER BY name");
    
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
    pqxx::read_transaction r(*conn_);
    auto result = r.exec_params("SELECT 1 FROM authors WHERE name = $1 LIMIT 1", name);
    return !result.empty();
}

PostgresBookRepository::PostgresBookRepository(std::shared_ptr<pqxx::connection> conn)
    : conn_(std::move(conn)) {
}

void PostgresBookRepository::Add(const domain::Book& book) {
    pqxx::work w(*conn_);
    w.exec_params(
        "INSERT INTO books (id, author_id, title, publication_year) VALUES ($1, $2, $3, $4)",
        UuidToString(book.id), UuidToString(book.author_id), book.title, book.publication_year
    );
    w.commit();
}

std::vector<domain::Book> PostgresBookRepository::GetByAuthorId(const domain::Id& author_id) {
    pqxx::read_transaction r(*conn_);
    auto result = r.exec_params(
        "SELECT id, title, publication_year FROM books "
        "WHERE author_id = $1 "
        "ORDER BY publication_year, title",
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

std::vector<domain::Book> PostgresBookRepository::GetAllSortedByTitle() {
    pqxx::read_transaction r(*conn_);
    auto result = r.exec("SELECT id, title, publication_year FROM books ORDER BY title");
    
    std::vector<domain::Book> books;
    for (const auto& row : result) {
        books.push_back(domain::Book{
            .id = StringToUuid(row[0].as<std::string>()),
            .author_id = domain::Id(),  // временно
            .title = row[1].as<std::string>(),
            .publication_year = row[2].as<int>()
        });
    }
    return books;
}

} // namespace infrastructure
