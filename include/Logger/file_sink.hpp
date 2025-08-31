#pragma once
#include <Logger/sink.hpp>
#include <fstream>
#include <mutex>

namespace logging {

class FileSink : public Sink {
public:
    static std::unique_ptr<Sink> create(const std::string& file_path);

    void write(const std::string& formatted_message, Level level) override;

private:
    FileSink(const std::string& file_path);

    std::ofstream file_;
    std::mutex mutex_;
    bool valid_{false};
};

} // namespace logging
