#pragma once
#include <boost/asio/io_context.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <memory>
#include <functional>

namespace http_server {

namespace net = boost::asio;
namespace beast = boost::beast;
namespace http = beast::http;
using tcp = net::ip::tcp;

template <typename RequestHandler>
class ServeHttp {
public:
    ServeHttp(net::io_context& ioc, const tcp::endpoint& endpoint, RequestHandler& handler)
        : ioc_(ioc), acceptor_(ioc), handler_(handler) {
        acceptor_.open(endpoint.protocol());
        acceptor_.set_option(net::socket_base::reuse_address(true));
        acceptor_.bind(endpoint);
        acceptor_.listen();
    }

    void Run() { DoAccept(); }
    void Stop() { acceptor_.close(); }

private:
    void DoAccept() {
        acceptor_.async_accept([this](beast::error_code ec, tcp::socket socket) {
            if (!ec) {
                std::make_shared<Session>(std::move(socket), handler_)->Run();
            }
            DoAccept();
        });
    }

    class Session : public std::enable_shared_from_this<Session> {
    public:
        Session(tcp::socket&& socket, RequestHandler& handler) 
            : socket_(std::move(socket)), handler_(handler) {}
        
        void Run() { DoRead(); }
        
    private:
        void DoRead() {
            auto self = shared_from_this();
            http::async_read(socket_, buffer_, req_,
                [self](beast::error_code ec, size_t) {
                    if (!ec) {
                        self->handler_(std::move(self->req_), 
                            [self](http::response<http::string_body>&& resp) {
                                self->DoWrite(std::move(resp));
                            });
                    }
                });
        }
        
        void DoWrite(http::response<http::string_body>&& resp) {
            auto self = shared_from_this();
            http::async_write(socket_, resp,
                [self](beast::error_code ec, size_t) {});
        }
        
        tcp::socket socket_;
        beast::flat_buffer buffer_;
        http::request<http::string_body> req_;
        RequestHandler& handler_;
    };

    net::io_context& ioc_;
    tcp::acceptor acceptor_;
    RequestHandler& handler_;
};

} // namespace http_server
