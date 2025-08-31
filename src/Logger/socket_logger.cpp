#include <Logger/socket_logger.hpp>
#include <Logger/socket_sink.hpp>

using namespace logging;

SocketLogger::SocketLogger(const std::string &host, int port, Level default_level) 
: Logger(default_level) {
    sinks_.push_back(SocketSink::create(host,port));
}