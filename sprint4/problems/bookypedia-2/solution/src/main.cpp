#include "database_manager.hpp"
#include "unit_of_work.hpp"
#include "bookypedia.hpp"
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
    std::cout << "  DeleteAuthor [name] - delete author and all his books" << std::endl;
    std::cout << "  EditAuthor [name] - edit author name" << std::endl;
    std::cout << "  ShowBook [title] - show book details with tags" << std::endl;
    std::cout << "  DeleteBook [title] - delete a book" << std::endl;
    std::cout << "  EditBook [title] - edit a book" << std::endl;
    std::cout << "  Help - show this help" << std::endl;
}

int main() {
    try {
        auto conn = std::make_shared<pqxx::connection>(GetDbUrl());
        
        infrastructure::DatabaseManager db_manager(conn);
        db_manager.InitializeTables();
        
        auto uow_factory = std::make_unique<infrastructure::UnitOfWorkFactory>(conn);
        
        application::Bookypedia app(std::move(uow_factory));
        
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
            else if (command == "DeleteAuthor") {
                std::string name;
                std::getline(iss, name);
                if (!name.empty() && name[0] == ' ') {
                    name = name.substr(1);
                }
                app.DeleteAuthor(name, std::cin, std::cout);
            }
            else if (command == "EditAuthor") {
                std::string name;
                std::getline(iss, name);
                if (!name.empty() && name[0] == ' ') {
                    name = name.substr(1);
                }
                app.EditAuthor(name, std::cin, std::cout);
            }
            else if (command == "ShowBook") {
                std::string title;
                std::getline(iss, title);
                if (!title.empty() && title[0] == ' ') {
                    title = title.substr(1);
                }
                app.ShowBook(title, std::cin, std::cout);
            }
            else if (command == "DeleteBook") {
                std::string title;
                std::getline(iss, title);
                if (!title.empty() && title[0] == ' ') {
                    title = title.substr(1);
                }
                app.DeleteBook(title, std::cin, std::cout);
            }
            else if (command == "EditBook") {
                std::string title;
                std::getline(iss, title);
                if (!title.empty() && title[0] == ' ') {
                    title = title.substr(1);
                }
                app.EditBook(title, std::cin, std::cout);
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
