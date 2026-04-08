#include "sdk.h"
//
#include <boost/asio/io_context.hpp>
#include <boost/asio/signal_set.hpp>
#include <iostream>
#include <thread>
#include <filesystem>

#include "json_loader.h"
#include "request_handler.h"
#include "logger.h"
#include "http_server.h"
#include "game_handler.h"
#include "auth_handler.h"
#include "players.h"
#include "player_tokens.h"

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

        model::Players players;
        model::PlayerTokens player_tokens;

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
        http_handler::GameHandler game_handler{players, player_tokens, game};
        http_handler::AuthHandler auth_handler{player_tokens, players};

        const auto address = net::ip::make_address("0.0.0.0");
        constexpr net::ip::port_type port = 8080;
        
        LogStartup(address.to_string(), port);
        
        std::cout << "Starting HTTP server on " << address << ":" << port << std::endl;
        
        http_server::ServeHttp(ioc, {address, port}, [&](auto&& req, auto&& send) {
            std::string target(req.target().data(), req.target().size());
            
            std::cout << "=== Router: target=" << target << std::endl;
            
            if (target == "/api/v1/game/join") {
                std::cout << "=== Routing to HandleJoinGame ===" << std::endl;
                auto response = game_handler.HandleJoinGame(req);
                send(std::move(response));
                return;
            }
            else if (target == "/api/v1/game/players") {
                std::cout << "=== Routing to HandleGetPlayers ===" << std::endl;
                auto it = req.find(boost::beast::http::field::authorization);
                if (it == req.end()) {
                    auto response = game_handler.MakeErrorResponse(
                        boost::beast::http::status::unauthorized, 
                        "invalidToken", 
                        "Authorization header is missing");
                    send(std::move(response));
                    return;
                }
                
                auto player_id = auth_handler.Authenticate(std::string(it->value()));
                if (!player_id) {
                    auto response = game_handler.MakeErrorResponse(
                        boost::beast::http::status::unauthorized, 
                        "invalidToken", 
                        "Invalid token");
                    send(std::move(response));
                    return;
                }
                
                auto response = game_handler.HandleGetPlayers(req, *player_id);
                send(std::move(response));
                return;
            }
            else if (target.size() >= 4 && target.substr(0, 4) == "/api") {
                std::cout << "=== Routing to API handler ===" << std::endl;
                handler(std::forward<decltype(req)>(req), std::forward<decltype(send)>(send));
            } else {
                std::cout << "=== Routing to static file handler ===" << std::endl;
                handler(std::forward<decltype(req)>(req), std::forward<decltype(send)>(send));
            }
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
