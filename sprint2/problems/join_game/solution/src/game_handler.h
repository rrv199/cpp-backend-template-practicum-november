#pragma once

#include <boost/beast/http.hpp>
#include <boost/json.hpp>
#include <iostream>
#include "players.h"
#include "player_tokens.h"

namespace http_handler {

namespace beast = boost::beast;
namespace http = beast::http;
namespace json = boost::json;

class GameHandler {
public:
    GameHandler(model::Players& players, model::PlayerTokens& tokens, model::Game& game)
        : players_(players), tokens_(tokens), game_(game) {}

    http::response<http::string_body> HandleJoinGame(const http::request<http::string_body>& req) {
        std::cout << "=== HandleJoinGame called ===" << std::endl;
        std::cout << "Method: " << req.method_string() << std::endl;
        std::cout << "Body: " << req.body() << std::endl;
        
        // Проверка метода
        if (req.method() != http::verb::post) {
            std::cout << "Method not POST" << std::endl;
            return MakeErrorResponse(http::status::method_not_allowed,
                "invalidMethod", "Only POST method is expected",
                {{"Allow", "POST"}});
        }

        // Проверка Content-Type
        auto it = req.find(http::field::content_type);
        if (it == req.end() || it->value() != "application/json") {
            std::cout << "Invalid Content-Type: " << (it == req.end() ? "missing" : std::string(it->value())) << std::endl;
            return MakeErrorResponse(http::status::bad_request,
                "invalidArgument", "Invalid Content-Type");
        }

        // Парсинг JSON
        json::value body;
        try {
            body = json::parse(req.body());
        } catch (const std::exception& e) {
            std::cout << "JSON parse error: " << e.what() << std::endl;
            return MakeErrorResponse(http::status::bad_request,
                "invalidArgument", "Join game request parse error");
        }

        // Извлечение полей
        auto& obj = body.as_object();
        if (!obj.contains("userName") || !obj.contains("mapId")) {
            std::cout << "Missing fields" << std::endl;
            return MakeErrorResponse(http::status::bad_request,
                "invalidArgument", "Missing required fields");
        }

        std::string user_name = std::string(obj.at("userName").as_string());
        std::string map_id_str = std::string(obj.at("mapId").as_string());

        std::cout << "userName: " << user_name << ", mapId: " << map_id_str << std::endl;

        // Проверка имени
        if (user_name.empty()) {
            std::cout << "Empty user name" << std::endl;
            return MakeErrorResponse(http::status::bad_request,
                "invalidArgument", "Invalid name");
        }

        // Проверка карты
        model::Map::Id map_id(map_id_str);
        const model::Map* map = game_.FindMap(map_id);
        if (!map) {
            std::cout << "Map not found: " << map_id_str << std::endl;
            return MakeErrorResponse(http::status::not_found,
                "mapNotFound", "Map not found");
        }

        // Создание игрока
        model::PlayerId player_id = players_.AddPlayer(user_name, map_id);
        model::Token token = tokens_.GenerateToken(player_id);

        std::cout << "Player created: id=" << *player_id << ", token=" << *token << std::endl;

        // Формирование ответа
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

        std::cout << "=== HandleGetPlayers called ===" << std::endl;
        std::cout << "Method: " << req.method_string() << std::endl;

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
    model::Game& game_;
};

}  // namespace http_handler
