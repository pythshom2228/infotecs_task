#include <LogApp/file_logger.hpp>
#include <Logger/file_sink.hpp>
#include <iostream>

using namespace logging;

FileLogger::FileLogger(const std::string& logfile, Level default_level)
    : logger_(logfile,default_level)
{
    worker_thread_ = std::thread(&FileLogger::worker_loop, this);
}

FileLogger::~FileLogger() {
    queue_.stop();
    if (worker_thread_.joinable())
        worker_thread_.join();
}

void FileLogger::run_console() {
    std::string message;
    std::string level_str;

    while (true) {
        std::cout << "Введите сообщение (или 'quit' для выхода): ";
        if (!std::getline(std::cin, message)) break;
        if (message == "quit") break;

        std::cout << "Введите уровень (info|warn|error, Enter = info): ";
        if (!std::getline(std::cin, level_str)) break;
        std::cout << '\n';
        Level lvl = Level::Info;
        if (!level_str.empty()) {
            lvl = parse_level(level_str);
        }

        queue_.push(Task{message, lvl});
    }
    queue_.stop();
}

void FileLogger::worker_loop() {
    while (true) {
        std::optional<Task> task_opt = queue_.pop();
        if(!task_opt) break;
        logger_.log(task_opt->message, task_opt->level);
    }
}

Level FileLogger::parse_level(const std::string& token) {
    if (token == "error" || token == "2") return Level::Error;
    else if (token == "warn"  || token == "1") return Level::Warning;
    else if (token == "info"  || token == "0") return Level::Info;
    return Level::Info; // если не распознали
}

