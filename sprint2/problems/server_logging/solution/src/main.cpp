#include "sdk.h"
//
#include <boost/asio/io_context.hpp>
#include <boost/asio/signal_set.hpp>
#include <iostream>
#include <thread>
#include <filesystem>

#include "json_loader.h"
#include "request_handler.h"
#include "logging_handler.h"
#include "logger.h"
#include "http_server.h"

using namespace std::literals;
namespace net = boost::asio;
namespace fs = std::filesystem;

namespace {

template <typename Fn>
void RunWorkers(unsigned n, const Fn& fn) {
    n = std::max(1u, n);
    std::vector<std::jthread> workers;
    workers.reserve(n - 1);
    while (--n) {
        workers.emplace_back(fn);
    }
    fn();
}

} // namespace

int main(int argc, const char* argv[]) {
    if (argc != 3) {
        std::cerr << "Usage: game_server <game-config-json> <static-root>"sv << std::endl;
        return EXIT_FAILURE;
    }
    
    try {
        InitLogger();
        
        model::Game game = json_loader::LoadGame(argv[1]);
        fs::path static_root = argv[2];

        std::cout << "Loaded " << game.GetMaps().size() << " maps" << std::endl;
        std::cout << "Static root: " << static_root << std::endl;

        const unsigned num_threads = std::thread::hardware_concurrency();
        net::io_context ioc(num_threads);

        net::signal_set signals(ioc, SIGINT, SIGTERM);
        signals.async_wait([&ioc](const boost::system::error_code& ec, int signal_number) {
            if (!ec) {
                LogShutdown(0);
                ioc.stop();
            }
        });

        http_handler::RequestHandler handler{game, static_root};
        http_handler::LoggingHandler<http_handler::RequestHandler> logging_handler{handler};

        const auto address = net::ip::make_address("0.0.0.0");
        constexpr net::ip::port_type port = 8080;
        
        LogStartup(address.to_string(), port);
        
        std::cout << "Starting HTTP server on " << address << ":" << port << std::endl;
        
        http_server::ServeHttp(ioc, {address, port}, [&logging_handler](auto&& req, auto&& send) {
            logging_handler(std::forward<decltype(req)>(req), std::forward<decltype(send)>(send));
        });

        std::cout << "Server has started..."sv << std::endl;

        RunWorkers(std::max(1u, num_threads), [&ioc] {
            ioc.run();
        });
        
        std::cout << "Server stopped" << std::endl;
        
    } catch (const std::exception& ex) {
        LogShutdown(EXIT_FAILURE, ex.what());
        std::cerr << "Error: " << ex.what() << std::endl;
        return EXIT_FAILURE;
    }
    
    LogShutdown(EXIT_SUCCESS);
    return EXIT_SUCCESS;
}
