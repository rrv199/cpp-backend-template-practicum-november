#pragma once

#include "request_handler.h"
#include "logger.h"
#include <chrono>

namespace http_handler {

template<typename Handler>
class LoggingHandler {
public:
    explicit LoggingHandler(Handler& handler) : handler_(handler) {}
    
template<typename Request, typename Send>
void operator()(Request&& req, Send&& send) {
    std::cout << "=== LoggingHandler::operator() START ===" << std::endl;
    std::string uri(req.target().data(), req.target().size());
    std::string method = std::string(req.method_string());
    
    std::cout << "=== URI: " << uri << ", Method: " << method << std::endl;
    
    LogRequest("unknown", uri, method);
    
    auto start = std::chrono::steady_clock::now();
    
    auto wrapped_send = [&, this](auto&& response) {
        std::cout << "=== Wrapped send called ===" << std::endl;
        auto end = std::chrono::steady_clock::now();
        auto response_time = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
        
        int code = response.result_int();
        std::string content_type;
        auto it = response.find(boost::beast::http::field::content_type);
        if (it != response.end()) {
            content_type = std::string(it->value().data(), it->value().size());
        }
        
        LogResponse("unknown", response_time, code, content_type);
        
        send(std::move(response));
    };
    
    std::cout << "=== Calling wrapped handler ===" << std::endl;
    handler_(std::forward<Request>(req), wrapped_send);
    std::cout << "=== LoggingHandler::operator() END ===" << std::endl;
}    
private:
    Handler& handler_;
};

} // namespace http_handler
