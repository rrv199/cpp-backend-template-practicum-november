#pragma once

#include <boost/log/trivial.hpp>
#include <boost/log/core.hpp>
#include <boost/log/expressions.hpp>
#include <boost/log/utility/setup/console.hpp>
#include <boost/log/utility/setup/common_attributes.hpp>
#include <boost/log/support/date_time.hpp>
#include <string>

namespace logging = boost::log;
namespace keywords = boost::log::keywords;
namespace expr = boost::log::expressions;

inline void InitLogger() {
    logging::add_common_attributes();
    
    logging::add_console_log(
        std::cout,
        keywords::format = "%TimeStamp%: %Message%",
        keywords::auto_flush = true
    );
}

inline void LogStartup(const std::string& address, unsigned short port) {
    BOOST_LOG_TRIVIAL(info) << "server started on " << address << ":" << port;
}

inline void LogShutdown(int code, const std::string& exception = "") {
    if (exception.empty()) {
        BOOST_LOG_TRIVIAL(info) << "server exited with code " << code;
    } else {
        BOOST_LOG_TRIVIAL(info) << "server exited with code " << code << ": " << exception;
    }
}

inline void LogRequest(const std::string& ip, const std::string& uri, const std::string& method) {
    BOOST_LOG_TRIVIAL(info) << "request: " << method << " " << uri << " from " << ip;
}

inline void LogResponse(const std::string& ip, long long response_time, int code, const std::string& content_type) {
    BOOST_LOG_TRIVIAL(info) << "response: " << code << " (" << content_type << ") to " << ip << " in " << response_time << "ms";
}

inline void LogError(int code, const std::string& text, const std::string& where) {
    BOOST_LOG_TRIVIAL(error) << "error: " << where << " - " << text << " (" << code << ")";
}
