#include <Logger/file_logger.hpp>
#include <Logger/file_sink.hpp>

using namespace logging;

FileLogger::FileLogger(const std::string& logfile, Level default_level) 
:   Logger(default_level) {
    sinks_.push_back(FileSink::create(logfile));
}