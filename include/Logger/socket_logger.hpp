#pragma once
#include <Logger/logger.hpp>

namespace logging {

class SocketLogger : public Logger {
public:
    explicit SocketLogger(const std::string& host, int port, Level default_level);
};

}