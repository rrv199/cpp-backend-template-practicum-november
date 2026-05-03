#include "http_server.h"
#include <iostream>

namespace http_server {

void SessionBase::Run() {
    auto self = shared_from_this();
    net::post(stream_.get_executor(), [self]() {
        self->Read();
    });
}

void SessionBase::Close() {
    beast::error_code ec;
    stream_.socket().shutdown(tcp::socket::shutdown_send, ec);
    if (ec) {
        std::cerr << "Shutdown error: " << ec.message() << std::endl;
    }
}

} // namespace http_server
