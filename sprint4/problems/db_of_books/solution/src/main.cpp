#include <iostream>
#include <pqxx/pqxx>
#include <string>
#include <optional>
#include <nlohmann/json.hpp>  // В conan нужно будет добавить

using json = nlohmann::json;
using namespace std::literals;
using pqxx::operator"" _zv;

class BookManager {
public:
    BookManager(const std::string& conn_string) : conn_(conn_string) {
        CreateTableIfNotExists();
        PrepareStatements();
    }
    
    void Run() {
        std::string line;
        while (std::getline(std::cin, line)) {
            try {
                if (line.empty()) continue;
                
                json request = json::parse(line);
                std::string action = request["action"];
                
                if (action == "add_book") {
                    HandleAddBook(request["payload"]);
                } else if (action == "all_books") {
                    HandleAllBooks();
                } else if (action == "exit") {
                    break;
                }
            } catch (const json::parse_error& e) {
                std::cerr << "JSON parse error: " << e.what() << std::endl;
            } catch (const pqxx::sql_error& e) {
                std::cerr << "SQL error: " << e.what() << std::endl;
                std::cout << json{{"result", false}}.dump() << std::endl;
            } catch (const std::exception& e) {
                std::cerr << "Error: " << e.what() << std::endl;
                std::cout << json{{"result", false}}.dump() << std::endl;
            }
        }
    }
    
private:
    void CreateTableIfNotExists() {
        pqxx::work w(conn_);
        w.exec(
            "CREATE TABLE IF NOT EXISTS books ("
            "id SERIAL PRIMARY KEY, "
            "title varchar(100) NOT NULL, "
            "author varchar(100) NOT NULL, "
            "year integer NOT NULL, "
            "ISBN char(13) UNIQUE"
            ");"_zv);
        w.commit();
    }
    
    void PrepareStatements() {
        conn_.prepare("insert_book",
            "INSERT INTO books (title, author, year, ISBN) "
            "VALUES ($1, $2, $3, $4) RETURNING id;"_zv);
        
        conn_.prepare("select_all_books",
            "SELECT id, title, author, year, ISBN "
            "FROM books "
            "ORDER BY year DESC, title ASC, author ASC, ISBN ASC;"_zv);
    }
    
    void HandleAddBook(const json& payload) {
        std::string title = payload["title"];
        std::string author = payload["author"];
        int year = payload["year"];
        
        std::optional<std::string> isbn;
        if (!payload["ISBN"].is_null()) {
            isbn = payload["ISBN"].get<std::string>();
        }
        
        try {
            pqxx::work w(conn_);
            pqxx::result result;
            
            if (isbn.has_value()) {
                result = w.exec_prepared("insert_book", title, author, year, isbn.value());
            } else {
                result = w.exec_prepared("insert_book", title, author, year, nullptr);
            }
            
            w.commit();
            
            if (!result.empty()) {
                std::cout << json{{"result", true}}.dump() << std::endl;
            } else {
                std::cout << json{{"result", false}}.dump() << std::endl;
            }
        } catch (const pqxx::sql_error& e) {
            // Ошибка уникальности ISBN или другая SQL ошибка
            std::cout << json{{"result", false}}.dump() << std::endl;
            throw;
        }
    }
    
    void HandleAllBooks() {
        pqxx::read_transaction r(conn_);
        pqxx::result result = r.exec_prepared("select_all_books");
        
        json books_array = json::array();
        
        for (const auto& row : result) {
            json book;
            book["id"] = row["id"].as<int>();
            book["title"] = row["title"].as<std::string>();
            book["author"] = row["author"].as<std::string>();
            book["year"] = row["year"].as<int>();
            
            if (row["ISBN"].is_null()) {
                book["ISBN"] = nullptr;
            } else {
                book["ISBN"] = row["ISBN"].as<std::string>();
            }
            
            books_array.push_back(book);
        }
        
        std::cout << books_array.dump() << std::endl;
    }
    
private:
    pqxx::connection conn_;
};

int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " <connection-string>" << std::endl;
        return EXIT_FAILURE;
    }
    
    try {
        BookManager manager(argv[1]);
        manager.Run();
    } catch (const std::exception& e) {
        std::cerr << "Fatal error: " << e.what() << std::endl;
        return EXIT_FAILURE;
    }
    
    return EXIT_SUCCESS;
}
