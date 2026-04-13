#include "infrastructure/database_manager.hpp"
#include "infrastructure/repositories.hpp"
#include "application/use_cases.hpp"
#include <iostream>
#include <memory>
#include <cstdlib>
#include <sstream>

std::string GetDbUrl() {
    const char* url = std::getenv("BOOKYPEDIA_DB_URL");
    if (!url) {
        std::cerr << "BOOKYPEDIA_DB_URL environment variable not set" << std::endl;
        exit(EXIT_FAILURE);
    }
    return std::string(url);
}

void PrintHelp() {
    std::cout << "Available commands:" << std::endl;
    std::cout << "  AddAuthor <name> - add a new author" << std::endl;
    std::cout << "  ShowAuthors - show all authors" << std::endl;
    std::cout << "  AddBook <year> <title> - add a new book" << std::endl;
    std::cout << "  ShowAuthorBooks - show books by selected author" << std::endl;
    std::cout << "  ShowBooks - show all books" << std::endl;
    std::cout << "  Help - show this help" << std::endl;
}

int main() {
    try {
        auto conn = std::make_shared<pqxx::connection>(GetDbUrl());
        
        // Инициализируем таблицы
        infrastructure::DatabaseManager db_manager(conn);
        db_manager.InitializeTables();
        
        // Создаём репозитории
        auto author_repo = std::make_unique<infrastructure::PostgresAuthorRepository>(conn);
        auto book_repo = std::make_unique<infrastructure::PostgresBookRepository>(conn);
        
        // Создаём приложение
        application::Bookypedia app(
            std::move(author_repo),
            std::move(book_repo)
        );
        
        // Обрабатываем команды
        std::string line;
        while (std::getline(std::cin, line)) {
            if (line.empty()) continue;
            
            std::istringstream iss(line);
            std::string command;
            iss >> command;
            
            if (command == "AddAuthor") {
                std::string name;
                std::getline(iss, name);
                if (!name.empty() && name[0] == ' ') {
                    name = name.substr(1);
                }
                app.AddAuthor(name);
            }
            else if (command == "ShowAuthors") {
                app.ShowAuthors();
            }
            else if (command == "AddBook") {
                int year;
                std::string title;
                iss >> year;
                std::getline(iss, title);
                if (!title.empty() && title[0] == ' ') {
                    title = title.substr(1);
                }
                app.AddBook(year, title, std::cin, std::cout);
            }
            else if (command == "ShowAuthorBooks") {
                app.ShowAuthorBooks(std::cin, std::cout);
            }
            else if (command == "ShowBooks") {
                app.ShowBooks();
            }
            else if (command == "Help") {
                PrintHelp();
            }
        }
        
    } catch (const std::exception& e) {
        std::cerr << "Fatal error: " << e.what() << std::endl;
        return EXIT_FAILURE;
    }
    
    return EXIT_SUCCESS;
}
