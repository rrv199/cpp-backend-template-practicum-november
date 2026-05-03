#include "request_handler.h"
#include <boost/beast/http.hpp>
#include <boost/json.hpp>
#include <iostream>

namespace http_handler {

namespace beast = boost::beast;
namespace http = beast::http;
namespace json = boost::json;

void RequestHandler::operator()(http::request<http::string_body>&& req,
                                std::function<void(http::response<http::string_body>&&)>&& send) {
    std::string target(req.target().data(), req.target().size());
    std::cout << "=== Request: " << target << std::endl;
    
    http::response<http::string_body> response;
    
    if (target == "/api/v1/maps") {
        json::array maps_array;
        std::string body = json::serialize(maps_array);
        response.result(http::status::ok);
        response.set(http::field::content_type, "application/json");
        response.body() = body;
    } else if (target == "/api/v1/game/records") {
        json::array records_array;
        std::string body = json::serialize(records_array);
        response.result(http::status::ok);
        response.set(http::field::content_type, "application/json");
        response.set(http::field::cache_control, "no-cache");
        response.body() = body;
    } else {
        response.result(http::status::not_found);
        response.body() = "Not found";
    }
    
    response.prepare_payload();
    send(std::move(response));
}

} // namespace http_handler
