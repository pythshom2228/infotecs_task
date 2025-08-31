#include <Logger/file_sink.hpp>

namespace logging {

std::unique_ptr<Sink> FileSink::create(const std::string& file_path) {
    return std::unique_ptr<FileSink>(new FileSink(file_path));
}

FileSink::FileSink(const std::string& file_path) {
    file_.open(file_path, std::ios::out | std::ios::trunc);
    if (file_.is_open()) {
        valid_ = true;
    }
}

void FileSink::write(const std::string& formatted_message, Level) {
    if (!valid_) return;
    std::lock_guard<std::mutex> lock(mutex_);
    file_ << formatted_message << "\n";
    file_.flush();
}

} // namespace logging
