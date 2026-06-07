#include "request_handler.h"
#include <boost/beast/http.hpp>
#include <iostream>
#include <string>

namespace http_handler {

namespace beast = boost::beast;
namespace http = beast::http;

using Response = http::response<http::string_body>;

void RequestHandler::operator()(http::request<http::string_body>&& req, std::function<void(http::response<http::string_body>&&)>&& send) {
    std::string target(req.target().data(), req.target().size());
    std::cout << "=== Request received: " << target << std::endl;
    std::cout << "=== Step 1" << std::endl;

    Response response;

    if (target == "/api/v1/maps") {
        std::cout << "=== Step 2: maps" << std::endl;
        response = HandleMapsRequest();
    }
    else if (target.size() > 13 && target.substr(0, 13) == "/api/v1/maps/") {
        std::cout << "=== Step 2: map detail" << std::endl;
        std::string map_id = target.substr(13);
        response = HandleMapRequest(map_id);
    }
    else if (target == "/api/v1/game/records") {
        std::cout << "=== Step 2: records" << std::endl;
        std::cout << "=== Step 3: creating response" << std::endl;
        
        response = Response(http::status::ok, 11);
        std::cout << "=== Step 4: response created" << std::endl;
        
        response.set(http::field::content_type, "application/json");
        std::cout << "=== Step 5: content_type set" << std::endl;
        
        response.set(http::field::cache_control, "no-cache");
        std::cout << "=== Step 6: cache_control set" << std::endl;
        
        response.body() = "[]";
        std::cout << "=== Step 7: body set" << std::endl;
        
        response.content_length(2);
        std::cout << "=== Step 8: content_length set" << std::endl;
        
        response.keep_alive(false);
        std::cout << "=== Step 9: keep_alive set" << std::endl;
        
        std::cout << "=== Records response sent" << std::endl;
    }
    else if (target.size() >= 4 && target.substr(0, 4) == "/api") {
        std::cout << "=== Step 2: api error" << std::endl;
        response = MakeErrorResponse(http::status::bad_request, "badRequest", "Bad request");
    }
    else {
        std::cout << "=== Step 2: not found" << std::endl;
        response = MakeErrorResponse(http::status::not_found, "notFound", "Not found");
    }

    std::cout << "=== Step 10: sending response" << std::endl;
    send(std::move(response));
    std::cout << "=== Step 11: response sent" << std::endl;
}

Response RequestHandler::HandleMapsRequest() {
    Response response(http::status::ok, 11);
    response.set(http::field::content_type, "application/json");
    response.body() = "[]";
    response.content_length(2);
    response.keep_alive(false);
    return response;
}

Response RequestHandler::HandleMapRequest(const std::string& map_id_str) {
    Response response(http::status::ok, 11);
    response.set(http::field::content_type, "application/json");
    response.body() = "{}";
    response.content_length(2);
    response.keep_alive(false);
    return response;
}

Response RequestHandler::MakeErrorResponse(http::status status, const std::string& code, const std::string& message) {
    Response response(status, 11);
    response.set(http::field::content_type, "application/json");
    response.body() = "{\"code\":\"" + code + "\",\"message\":\"" + message + "\"}";
    response.content_length(response.body().size());
    response.keep_alive(false);
    return response;
}

} // namespace http_handler
