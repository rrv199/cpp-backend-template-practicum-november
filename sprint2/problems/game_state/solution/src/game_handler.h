#pragma once

#include <boost/beast/http.hpp>
#include <boost/json.hpp>
#include <iostream>
#include "players.h"
#include "player_tokens.h"
#include "dogs.h"
#include "position_generator.h"

namespace http_handler {

namespace beast = boost::beast;
namespace http = beast::http;
namespace json = boost::json;

class GameHandler {
public:
    GameHandler(model::Players& players, 
                model::PlayerTokens& tokens, 
                model::Dogs& dogs,
                model::Game& game)
        : players_(players), tokens_(tokens), dogs_(dogs), game_(game) {}

    http::response<http::string_body> HandleJoinGame(const http::request<http::string_body>& req) {
        std::cout << "=== HandleJoinGame called ===" << std::endl;
        std::cout << "Method: " << req.method_string() << std::endl;
        std::cout << "Body: " << req.body() << std::endl;
        
        if (req.method() != http::verb::post) {
            return MakeErrorResponse(http::status::method_not_allowed,
                "invalidMethod", "Only POST method is expected",
                {{"Allow", "POST"}});
        }

        auto it = req.find(http::field::content_type);
        if (it == req.end() || it->value() != "application/json") {
            return MakeErrorResponse(http::status::bad_request,
                "invalidArgument", "Invalid Content-Type");
        }

        json::value body;
        try {
            body = json::parse(req.body());
        } catch (const std::exception& e) {
            return MakeErrorResponse(http::status::bad_request,
                "invalidArgument", "Join game request parse error");
        }

        auto& obj = body.as_object();
        if (!obj.contains("userName") || !obj.contains("mapId")) {
            return MakeErrorResponse(http::status::bad_request,
                "invalidArgument", "Missing required fields");
        }

        std::string user_name = std::string(obj.at("userName").as_string());
        std::string map_id_str = std::string(obj.at("mapId").as_string());

        if (user_name.empty()) {
            return MakeErrorResponse(http::status::bad_request,
                "invalidArgument", "Invalid name");
        }

        model::Map::Id map_id(map_id_str);
        const model::Map* map = game_.FindMap(map_id);
        if (!map) {
            return MakeErrorResponse(http::status::not_found,
                "mapNotFound", "Map not found");
        }

        model::Point pos = position_generator_.GeneratePosition(*map);
        model::DogId dog_id = dogs_.AddDog(user_name, pos);
        model::PlayerId player_id = players_.AddPlayer(user_name, map_id, dog_id);
        model::Token token = tokens_.GenerateToken(player_id);

        json::object response_obj;
        response_obj["authToken"] = *token;
        response_obj["playerId"] = *player_id;

        std::string body_str = json::serialize(response_obj);

        http::response<http::string_body> response(http::status::ok, 11);
        response.set(http::field::content_type, "application/json");
        response.set(http::field::cache_control, "no-cache");
        response.body() = body_str;
        response.content_length(body_str.size());
        response.keep_alive(false);

        return response;
    }

    http::response<http::string_body> HandleGetPlayers(
        const http::request<http::string_body>& req,
        model::PlayerId player_id) {

        if (req.method() != http::verb::get && req.method() != http::verb::head) {
            return MakeErrorResponse(http::status::method_not_allowed,
                "invalidMethod", "Invalid method",
                {{"Allow", "GET, HEAD"}});
        }

        const model::Player* player = players_.GetPlayer(player_id);
        if (!player) {
            return MakeErrorResponse(http::status::unauthorized,
                "unknownToken", "Player token has not been found");
        }

        json::object players_obj;
        for (const auto& [id, p] : players_.GetAllPlayers()) {
            if (p.GetMapId() == player->GetMapId()) {
                json::object player_info;
                player_info["name"] = p.GetName();
                players_obj[std::to_string(*id)] = player_info;
            }
        }

        std::string body_str = json::serialize(players_obj);

        http::response<http::string_body> response(http::status::ok, 11);
        response.set(http::field::content_type, "application/json");
        response.set(http::field::cache_control, "no-cache");
        response.body() = body_str;
        response.content_length(body_str.size());
        response.keep_alive(false);

        return response;
    }

    http::response<http::string_body> HandleGameState(
        const http::request<http::string_body>& req,
        model::PlayerId player_id) {

        if (req.method() != http::verb::get && req.method() != http::verb::head) {
            return MakeErrorResponse(http::status::method_not_allowed,
                "invalidMethod", "Invalid method",
                {{"Allow", "GET, HEAD"}});
        }

        const model::Player* player = players_.GetPlayer(player_id);
        if (!player) {
            return MakeErrorResponse(http::status::unauthorized,
                "unknownToken", "Player token has not been found");
        }

        const model::Dog* dog = dogs_.GetDog(player->GetDogId());
        if (!dog) {
            return MakeErrorResponse(http::status::internal_server_error,
                "internalError", "Dog not found");
        }

        json::object players_obj;
        json::object player_state;
        
        // Координаты
        json::array pos_array;
        pos_array.push_back(dog->GetPosition().x);
        pos_array.push_back(dog->GetPosition().y);
        player_state["pos"] = pos_array;
        
        // Скорость
        json::array speed_array;
        speed_array.push_back(dog->GetSpeed().vx);
        speed_array.push_back(dog->GetSpeed().vy);
        player_state["speed"] = speed_array;
        
        // Направление
        player_state["dir"] = model::DirectionToString(dog->GetDirection());
        
        players_obj[std::to_string(*player_id)] = player_state;

        json::object result;
        result["players"] = players_obj;

        std::string body_str = json::serialize(result);

        http::response<http::string_body> response(http::status::ok, 11);
        response.set(http::field::content_type, "application/json");
        response.set(http::field::cache_control, "no-cache");
        response.body() = body_str;
        response.content_length(body_str.size());
        response.keep_alive(false);

        return response;
    }

    http::response<http::string_body> MakeErrorResponse(
        http::status status,
        const std::string& code,
        const std::string& message,
        std::vector<std::pair<std::string, std::string>> extra_headers = {}) {

        json::object error_obj;
        error_obj["code"] = code;
        error_obj["message"] = message;

        std::string body = json::serialize(error_obj);

        http::response<http::string_body> response(status, 11);
        response.set(http::field::content_type, "application/json");
        response.set(http::field::cache_control, "no-cache");
        for (const auto& [key, value] : extra_headers) {
            response.set(key, value);
        }
        response.body() = body;
        response.content_length(body.size());
        response.keep_alive(false);

        return response;
    }

private:
    model::Players& players_;
    model::PlayerTokens& tokens_;
    model::Dogs& dogs_;
    model::Game& game_;
    model::PositionGenerator position_generator_;
};

}  // namespace http_handler
