#pragma once
#include <Logger/logger.hpp>

namespace logging {

class FileLogger : public Logger {
public:
    explicit FileLogger(const std::string& logfile, Level default_level);
};

}