#pragma once
#include "model.h"
#include <boost/beast/http.hpp>
#include <functional>
#include <string>

namespace http_handler {

class RequestHandler {
public:
    explicit RequestHandler(model::Game& game) : game_(game) {}
    
    void operator()(boost::beast::http::request<boost::beast::http::string_body>&& req,
                    std::function<void(boost::beast::http::response<boost::beast::http::string_body>&&)>&& send);
private:
    model::Game& game_;
};

} // namespace http_handler
