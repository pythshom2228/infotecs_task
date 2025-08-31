#include <Logger/logger.hpp>
#include <Logger/file_sink.hpp>
#include <sstream>
#include <ctime>

namespace logging {

Logger::Logger(Level default_level)
    : current_level_(default_level) {}

void Logger::add_sink(SinkPtr sink) {
    if (!sink) return;
    std::lock_guard<std::mutex> lock(mutex_);
    sinks_.push_back(std::move(sink));
}

void Logger::log(const std::string& message, Level level) {
    if (level < current_level_) return;

    std::string formatted = format_message(message, level);

    std::lock_guard<std::mutex> lock(mutex_);
    for (auto& sink : sinks_) {
        if (sink) {
            sink->write(formatted, level);
        }
    }
}

void Logger::set_level(Level level) {
    current_level_ = level;
}

std::string Logger::format_message(const std::string& message, Level level) {
    auto now = std::chrono::system_clock::now();
    std::time_t t = std::chrono::system_clock::to_time_t(now);
    std::tm tm{};
    localtime_r(&t, &tm);
    char buf[32];
    std::strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", &tm);

    std::ostringstream ss;
    ss << "[" << buf << "] "
       << "[" << (level == Level::Error ? "ERROR" :
                   level == Level::Warning ? "WARN" : "INFO") << "] "
       << message;
    return ss.str();
}

} // namespace logging
