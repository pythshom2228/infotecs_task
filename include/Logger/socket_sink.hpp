#pragma once
#include "sink.hpp"
#include <mutex>
#include <string>


namespace logging {

class SocketSink : public Sink {
public:
    // Возвращает всегда один объект на один host:port
    static std::unique_ptr<Sink> create(const std::string& host, int port);

    void write(const std::string& formatted_message, Level level) override;
    ~SocketSink();

private:
    SocketSink(const std::string& host, int port);

    int sock_{-1};
    std::mutex mutex_;
    bool valid_{false};
};

} // namespace logging
