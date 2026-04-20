#pragma once
#include "model.h"
#include <boost/beast/http.hpp>
#include <functional>
#include <string>

namespace http_handler {

class RequestHandler {
public:
    explicit RequestHandler(const model::Game& game) : game_(game) {}
    
    void operator()(boost::beast::http::request<boost::beast::http::string_body>&& req,
                    std::function<void(boost::beast::http::response<boost::beast::http::string_body>&&)>&& send);
    
private:
    const model::Game& game_;
    
    using Response = boost::beast::http::response<boost::beast::http::string_body>;
    
    Response HandleMapsRequest();
    Response HandleMapRequest(const std::string& map_id);
    Response HandleRecordsRequest();
    Response MakeErrorResponse(boost::beast::http::status status, const std::string& code, const std::string& message);
};

} // namespace http_handler
