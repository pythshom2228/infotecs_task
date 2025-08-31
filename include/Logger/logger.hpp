#pragma once
#include <Logger/sink.hpp>
#include <vector>
#include <mutex>
#include <atomic>

namespace logging {

class Logger {
public:
    using SinkPtr = std::unique_ptr<Sink>;

    explicit Logger(Level default_level = Level::Info);

    void add_sink(SinkPtr sink);

    void log(const std::string& message, Level level = Level::Info);
    void set_level(Level level);

protected:
    std::string format_message(const std::string& message, Level level);

    std::vector<SinkPtr> sinks_;
    std::mutex mutex_;
    std::atomic<Level> current_level_;
};

} // namespace logging
