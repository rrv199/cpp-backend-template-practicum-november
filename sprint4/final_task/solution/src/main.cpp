#include "http_server.h"
#include "json_loader.h"
#include "request_handler.h"
#include "database.hpp"

#include <boost/asio/io_context.hpp>
#include <boost/asio/signal_set.hpp>
#include <boost/asio/ip/address.hpp>
#include <iostream>
#include <thread>

namespace net = boost::asio;
using namespace std::literals;

int main(int argc, const char* argv[]) {
    if (argc != 2) {
        std::cerr << "Usage: game_server <game-config-json>" << std::endl;
        return EXIT_FAILURE;
    }

    try {
        auto game = json_loader::LoadGame(argv[1]);

        const char* db_url = std::getenv("GAME_DB_URL");
        if (!db_url) {
            std::cerr << "GAME_DB_URL environment variable not set" << std::endl;
            return EXIT_FAILURE;
        }
        
        database::DatabaseManager db_manager(db_url);
        db_manager.InitializeTables();

        http_handler::RequestHandler handler{game, db_manager};

        const auto address = net::ip::make_address("0.0.0.0");
        constexpr unsigned short port = 8080;

        net::io_context ioc(2);
        
        http_server::ServeHttp(ioc, {address, port}, handler);

        net::signal_set signals(ioc, SIGINT, SIGTERM);
        signals.async_wait([&ioc](const boost::system::error_code&, int) {
            ioc.stop();
        });

        std::cout << "Server started on port " << port << std::endl;
        ioc.run();

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
