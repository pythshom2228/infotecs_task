#pragma once
#include <string>
#include <memory>

namespace logging {

enum class Level {
    Info = 0,
    Warning,
    Error
};

// Интерфейс для "приёмника" логов
class Sink {
public:
    virtual ~Sink() = default;
    virtual void write(const std::string& formatted_message, Level level) = 0;
};

} // namespace logging
