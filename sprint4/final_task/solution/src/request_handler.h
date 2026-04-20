#pragma once
#include "model.h"
#include "database.hpp"
#include <boost/beast/http.hpp>
#include <functional>
#include <string>

namespace http_handler {

// Константы для эндпоинтов
namespace Endpoint {
    constexpr std::string_view MAPS = "/api/v1/maps";
    constexpr std::string_view MAP_PREFIX = "/api/v1/maps/";
    constexpr std::string_view RECORDS = "/api/v1/game/records";
    constexpr std::string_view API_PREFIX = "/api";
}

class RequestHandler {
public:
    explicit RequestHandler(const model::Game& game, database::DatabaseManager& db_manager)
        : game_(game), db_manager_(db_manager) {}
    
    void operator()(boost::beast::http::request<boost::beast::http::string_body>&& req,
                    std::function<void(boost::beast::http::response<boost::beast::http::string_body>&&)>&& send);
    
private:
    const model::Game& game_;
    database::DatabaseManager& db_manager_;
    
    using Response = boost::beast::http::response<boost::beast::http::string_body>;
    
    Response HandleMapsRequest();
    Response HandleMapRequest(const std::string& map_id);
    Response HandleRecordsRequest();
    Response MakeErrorResponse(boost::beast::http::status status, const std::string& code, const std::string& message);
};

} // namespace http_handler
