#pragma once
#include "model.h"
#include "players.h"
#include "player_tokens.h"
#include "dogs.h"
#include "position_generator.h"
#include "constants.h"
#include "endpoints.h"
#include <boost/beast/http.hpp>
#include <boost/json.hpp>
#include <chrono>
#include <optional>
#include <random>

namespace beast = boost::beast;
namespace http = beast::http;
namespace json = boost::json;

class GameHandler {
public:
    GameHandler(model::Players& players, 
                model::PlayerTokens& tokens,
                model::Dogs& dogs,
                const model::Game& game)
        : players_(players), tokens_(tokens), dogs_(dogs), game_(game) {}

    void SetRandomizeSpawnPoints(bool randomize) {
        pos_generator_.SetRandomize(randomize);
    }

    http::response<http::string_body> HandleJoinGame(const http::request<http::string_body>& req);
    http::response<http::string_body> HandleGetPlayers(const http::request<http::string_body>& req, model::PlayerId player_id);
    http::response<http::string_body> HandleGameState(const http::request<http::string_body>& req, model::PlayerId player_id);
    http::response<http::string_body> HandlePlayerAction(const http::request<http::string_body>& req, model::PlayerId player_id);
    http::response<http::string_body> HandleTick(const http::request<http::string_body>& req);
    
    void Tick(std::chrono::milliseconds delta);

    http::response<http::string_body> MakeErrorResponse(http::status status, const std::string& code, const std::string& message);

private:
    model::Players& players_;
    model::PlayerTokens& tokens_;
    model::Dogs& dogs_;
    const model::Game& game_;
    model::PositionGenerator pos_generator_;
};
